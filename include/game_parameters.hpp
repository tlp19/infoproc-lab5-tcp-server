#ifndef game_parameters_hpp
#define game_parameters_hpp

/*  ---  Game Parameters  ---  */

namespace Parameters {

   const int backlog=10;   // Number of pending connections before clients are refused.
                           // I.e. maximum number of player.
                           // Maximum: 1000

   const int angle_range=100;    // Range that X and Y angles will be generated in.
                                 // Angles are centered around zero, so actual range is [-angle_range/2 ; angle_range/2]
                                 // Recommanded: range value 100 (eq. to [-50 ; 50])
                                 // Maximum: 510 (eq. to [-255 ; 255])
}
#endif
