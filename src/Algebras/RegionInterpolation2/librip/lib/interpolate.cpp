/*
 1 interpolate.cpp is the home of the main interpolation recursion.
 
*/

#include "interpolate.h"

#include <string>


void handleIntersections(MFaces& children, MFace parent, bool evap);

// This variable holds the pointer to the matching strategy to use
vector<pair<Face*,Face*> > (*matchingStrategy)(vector<Face>*,vector<Face>*,
        int,string);

/*
   1.1 ~Interpolate~

   Main interpolation function between two lists of regions.
It calls the matching-function to create pairs of regions from the source- and
destination-list and interpolates the convex hulls of these regions using the
RotatingPlane algorithm. It then creates two new lists from the concavities and
holes of the region and recurses. The result of the recursion is then merged
into the current result. Intersections are detected and tried to be compensated

*/
MFaces interpolate(vector<Face> *sregs, vector<Face> *dregs, int depth,
        bool evap, string args) {
    MFaces ret;
    
    DEBUG(1, "== Entering Interpolate depth " << depth << " ==");
    
    if (sregs->empty() && dregs->empty()) // Nothing to do!
        return ret;
    
    // Remember the original faces-lists from which the result was created.
    ret.sregs = sregs;
    ret.dregs = dregs;


    // Match the faces to pairs of faces in the source- and destination-realm
    // If we are in the evaporation (or condensation) phase, just try to match
    // equal faces by their lower left point.
    vector<pair<Face *, Face *> > matches;
    if (!evap)
        matches = matchFaces(sregs, dregs, depth, args);
    else
        matches = matchFaces(sregs, dregs, depth, "lowerleft");
    
 
    for (unsigned int i = 0; i < matches.size(); i++) {
        pair<Face *, Face *> p = matches[i];

        Face *src = p.first;
        Face *dst = p.second;
        if (src && dst) {
 
            // Use the RotatingPlane-Algorithm to create an interpolation of
            // the convex hull of src and dst and identify all concavities
            RotatingPlane rp(src, dst, depth, evap);

            // Recurse and try to match and interpolate the list of concavities
            MFaces fcs = interpolate(&rp.scvs, &rp.dcvs, depth+1, evap, args);

            // Now check if the interpolations intersect in any way
            handleIntersections(fcs, rp.mface, evap);

            // Inherit if the interpolation needs a evaporisation- or
            // condensation-phase
            ret.needSEvap = ret.needSEvap || fcs.needSEvap;
            ret.needDEvap = ret.needDEvap || fcs.needDEvap;

            // Try to merge the recursively created moving segments with the
            // parent rp.mface
            for (unsigned int i = 0; i < fcs.faces.size(); i++) {
                // The holes of the faces are new faces now
                // Since they should be completely contained by their parent,
                // an intersection-check is unnecessary. Intersections with
                // each other were already checked one recursion level lower.
                for (unsigned int j = 0; j < fcs.faces[i].holes.size(); j++) {
                    MFace fc(fcs.faces[i].holes[j]);
                    ret.AddMFace(fc);
                }
                // Add the cycles to its parent
                rp.mface.AddConcavity(fcs.faces[i]);
            }

            // and try to merge them into the cycle here (otherwise they are
            // added as a hole)
            vector<MFace> splits = rp.mface.MergeConcavities();
            // Now the resulting moving face is added to the return value
            ret.AddMFace(rp.mface);
            for (unsigned int i = 0; i < splits.size(); i++)
                ret.AddMFace(splits[i]);
        } else {
            // Our face doesn't have a partner, so the recursion stops here and
            // we collapse (or expand) the face together with its holes
            Face *r = src ? src : dst;
            MFace coll = r->collapseWithHoles(r == src);
            ret.AddMFace(coll);
        }
    }

    // Toplevel-Intersections are still not handled yet, do that now.
    if (depth == 0) {
        handleIntersections(ret, MFace(), evap);
    }
    
    DEBUG(1, "== Leaving Interpolate depth " << depth << " ==");

    return ret;
}

/*
 1.2 ~handleIntersections~ checks the newly created moving faces ~children~ 
 for intersection with each other or their parent ~parent~.
 
 If a real interpolation intersects, then we remove it and collapse/expand the
 source- and destinationface of that moving face. If both intersecting moving
 faces are already collapsed or expanded, then one of them is removed and we
 need an evaporation- or condensationphase.
 If we currently are in the evaporation- or condensationphase, then ~evap~ is
 set and we handle intersections a little different.
 
*/
void handleIntersections(MFaces& children, MFace parent, bool evap) {
    vector<MSegs> evp; // The list of evaporation-mfaces
    
    // precalculate the bounding boxes of the children and the parent
    for (int i = 0; i < (int) children.faces.size(); i++) {
        children.faces[i].face.calculateBBox();
    }
    parent.face.calculateBBox();

    // Now check all pairs of faces of children and the parent, but skip
    // symmetric and reflexive pairs.
    for (int i = 0; i < (int) children.faces.size(); i++) {
        MSegs *s1 = &children.faces[i].face;
        for (int j = 0; j <= i; j++) {
            MSegs *s2 = (j == 0) ? &parent.face : &children.faces[j - 1].face;

            if (s1->intersects(*s2, false, false)) {
                // We have found two intersecting moving faces.
                pair<MSegs, MSegs> ss;
                if (!s1->iscollapsed && !evap) {
                    // If this moving face is not collapsed and we do not
                    // evaporate then break the connection and collapse/expand 
                    // the related faces.
                    ss = s1->kill(); // Create the pair of MSegs
                    // Remove the original object from the list
                    children.faces.erase(children.faces.begin()+i);
                } else if (!s2->iscollapsed && (s2 != &parent.face)
                        && !evap) {
                    // s1 already was collapsed but s2 is not, so this is the
                    // victim now. Refuse to remove the parent.
                    ss = s2->kill();
                    children.faces.erase(children.faces.begin() + (j-1));
                } else {
                    // Both intersecting objects are already collapsing or
                    // expanding (or we are evaporating right now)
                    MSegs *rm;
                    if (evap) {
                        // Evaporation- or Condensation-Phase.
                        // Minimum one of the two definitively is already
                        // collapsing/expanding, choose this one
                        if (s1->iscollapsed && !s1->isevaporating)
                            rm = s1;
                        else if (!s2->isevaporating && (s2 != &parent.face))
                            rm = s2;
                        else // cannot be a real intersection, continue
                            continue;
                        vector<MSegs> ms;
                        // iscollapsed is 1, if the mface is collapsing,
                        // otherwise (2) it is expanding and we have to
                        // condensate it
                        if (rm->iscollapsed == 1) {
                            ms = rm->sreg.Evaporate(true);
                        } else {
                            ms = rm->dreg.Evaporate(false);
                        }
                        // Insert the evaporation-cycles into the list
                        children.faces.insert(children.faces.end(),
                                              ms.begin(), ms.end());
                        evp.insert(evp.end(), ms.begin(), ms.end());
                    } else {
                        // We are in the main interpolation and cannot
                        // compensate this intersection. Remove one offending
                        // object and save, that we have to evaporate or
                        // condensate to fix that up.
                        rm = s1;
                        // iscollapsed is 1, if the mface is collapsing,
                        // otherwise (2) it is expanding and we have to
                        // do a condensation-phase
                        if (rm->iscollapsed == 1)
                            children.needSEvap = true;
                        else
                            children.needDEvap = true;
                    }
                    // Simply erase the offending object from the list now,
                    // either we have already created evaporisation/condensation
                    // cycles or we marked that we have to fix things up later
                    children.faces.erase(children.faces.begin()+
                                         (rm == s1 ? i : j - 1));
                    // Restart the checks with the last face, since we changed
                    // the lists
                    i--;
                    break;
                }
                // Add the collapse- and expand-cycles
                children.AddMFace(ss.first);
                children.AddMFace(ss.second);
                // Restart the checks one object earlier, since we removed
                // the current object and the following objects filled the gap.
                i-=2;
                break;
            }
        }
    }
    
    // Insert the evaporation- and condensation-cycles into the list of faces
//    children.faces.insert(children.faces.end(), evp.begin(), evp.end());
}

/*
 1.3 ~regioninterpolate~ is the interface to the interpolation library.
     
 Input:
        src   - Source region in nested list representation
        dst   - Destination region in nested list representation
        start - Timestamp of source region in ms since 1970-01-01 00:00
        end   - Timestamp of destination region in ms since 1970-01-01 00:00
        args  - Parameterstring for the face matching strategy
 
 Returns:
        The moving region in nested list representation

*/
RList regioninterpolate(RList src, RList dst, 
        string start, string end, string args) {

    vector<Face> sregs = Face::getFaces(src);
    vector<Face> dregs = Face::getFaces(dst);
    
    // Create the interpolation from the lists of faces
    MFaces mf = interpolate(&sregs, &dregs, 0, false, args);

    Interval iv(start, end, true, true);
    
    return mf.ToMListExpr(iv);
}

