#ifndef _XPEN_CV_H
#define _XPEN_CV_H


#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "../XGlobal.h"

#if CV_VERSION_EPOCH == 2
#  if CV_VERSION_MAJOR == 4
#    if CV_VERSION_MINOR == 12
#       ifdef _DEBUG
#         pragma comment(lib, "opencv_core2412d.lib")
#         pragma comment(lib, "opencv_imgproc2412d.lib")
#         pragma comment(lib, "opencv_highgui2412d.lib")
#       else
#         pragma comment(lib, "opencv_core2412.lib")
#         pragma comment(lib, "opencv_imgproc2412.lib")
#         pragma comment(lib, "opencv_highgui2412.lib")
#       endif
#    elif CV_VERSION_MINOR == 13
#      ifdef _DEBUG
#        pragma comment(lib, "opencv_core2413d.lib")
#        pragma comment(lib, "opencv_imgproc2413d.lib")
#        pragma comment(lib, "opencv_highgui2413d.lib")
#      else
#        pragma comment(lib, "opencv_core2413.lib")
#        pragma comment(lib, "opencv_imgproc2413.lib")
#        pragma comment(lib, "opencv_highgui2413.lib")
#      endif
#    endif
#  endif
#endif

#endif //
