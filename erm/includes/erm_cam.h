/**
 * erm_cam.h
 *
 * Dave Taht
 * 2017-03-20
 */

#ifndef ERM_CAM_H
#define ERM_CAM_H

// Logically the table lookup engine in erm looks a lot like a bunch of CAM
// memories, selected by popcount and plen as first order indexes to search.
// 32k CAMs. Which seems like a lot, to this old hardware fart. In software it's
// *nothing*, with a 16 bit index, and reduces the depth of the search by a lot.

// It's worse than that. They are generational and may have multiple copies extant.

// We can optimize further by only creating cams for 0,48,56,60,64,128 to match
// common ipv6 prefix lengths, and 0,8-24 for common ipv4 prefix lengths.

// And we can compile selectively to do a "hit" search on one or the other,
// which gets things down to a "reasonable in hardware" value - and then more
// exhaustively sweep the resulting table for a match. Anyway, the underlying
// structure of the cam is not relevant to the application, so the API is:

ermcam_t erm_cams_create(erm_t handle, int size, int number) COLD;
ermcam_t erm_cams_destroy(erm_t handle) COLD;

ermcam_t erm_cam_put(ermcam_t handle, ... ) HOT;
ermcam_t erm_cam_get(ermcam_t handle, ... ) VERYHOT;
ermcam_t erm_cam_tag(ermcam_t handle, ... ) VERYHOT;

#endif
