#ifndef ip_helpers_hpp
#define ip_helpers_hpp

/*  ---  Game Parameters  ---  */

extern int backlog=10;  // Number of pending connections before clients are refused.
                        // I.e. maximum number of player.

extern int angle_range=100;   // Range that X and Y angles will be generated in.
                              // Angles are centered around zero, so actual range is [-angle_range/2 ; angle_range/2]
                              // Recommanded: range value 100 (eq. to [-50 ; 50])
                              // Maximum: 510 (eq. to [-255 ; 255])

#endif
