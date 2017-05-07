#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <fstream>

std::string path = "images/";

IplImage* src;
IplImage* target;
IplImage* src_template_fix;
IplImage* template_image;
IplImage* matching_image;
IplImage* matching_result;

std::string filename;
std::string filenameTemplate;
std::string filenameTarget;
char filenameBuffer[21];

std::vector<std::string> filenameStack;

int maxImageID = 30;
int startImageID = 0;

int template_size = 30;
int template_search_size = 400;
int depth;
int channels;
int stackPosition = 0;

CvPoint minLoc;
CvPoint maxLoc;
double minVal;
double maxVal;

bool templateDefined = false;

int xPos;
int yPos;

void releaseMemory();
void mouseHandlerSingleMode(int event, int x, int y, int flags, void *param);
void locateSingleMode(int _x, int _y);
void mouseHandlerBatchMode(int event, int x, int y, int flags, void *param);
void locateBatchMode(int _x, int _y);
void fillImageStack();

int main(int _argc, char** _argv)
{

    if(_argv[1] && _argv[2])
    {
        printf("\n");
        printf("===================\n");
        printf("    SINGLE MODE \n");
        printf("===================\n\n");

        filenameTemplate = "";
        filenameTemplate.append(path.c_str());
        filenameTemplate.append(_argv[1]);

        filenameTarget = "";
        filenameTarget.append(path.c_str());
        filenameTarget.append(_argv[2]);

        src_template_fix = cvLoadImage(filenameTemplate.c_str(),1);

        depth = src_template_fix->depth;
        channels = src_template_fix->nChannels;

        cvNamedWindow("CHOOSE TEMPLATE",CV_WINDOW_NORMAL);
        cvShowImage("CHOOSE TEMPLATE", src_template_fix);
        cvSetMouseCallback("CHOOSE TEMPLATE", mouseHandlerSingleMode, (void*) src_template_fix);

        cvNamedWindow("TEMPLATE",CV_WINDOW_NORMAL);
        cvNamedWindow("TARGET IMAGE", CV_WINDOW_NORMAL);
        cvNamedWindow("MATCHING RESULT", CV_WINDOW_NORMAL);

        matching_result = cvCreateImage(cvSize(template_search_size - template_size + 1, template_search_size - template_size + 1), IPL_DEPTH_32F,1);
        template_image = cvCreateImage(cvSize(template_size, template_size),depth, channels);
        matching_image = cvCreateImage(cvSize(template_search_size,template_search_size),depth,channels);

        cvWaitKey(0);

    }
    else if(_argv[1] && !_argv[2])
    {
        printf("\n");
        printf("===================\n");
        printf("     BATCH MODE    \n");
        printf("===================\n\n");

        filenameTemplate = "";
        filenameTemplate.append(path.c_str());
        filenameTemplate.append(_argv[1]);

        src_template_fix = cvLoadImage(filenameTemplate.c_str(),1);

        depth = src_template_fix->depth;
        channels = src_template_fix->nChannels;

        cvNamedWindow("CHOOSE TEMPLATE",CV_WINDOW_NORMAL);
        cvShowImage("CHOOSE TEMPLATE", src_template_fix);
        cvSetMouseCallback("CHOOSE TEMPLATE", mouseHandlerBatchMode, (void*) src_template_fix);

        cvNamedWindow("TEMPLATE",CV_WINDOW_NORMAL);
        cvNamedWindow("TARGET IMAGE", CV_WINDOW_NORMAL);
        cvNamedWindow("MATCHING RESULT", CV_WINDOW_NORMAL);

        matching_result = cvCreateImage(cvSize(template_search_size - template_size + 1, template_search_size - template_size + 1), IPL_DEPTH_32F,1);
        template_image = cvCreateImage(cvSize(template_size, template_size),depth, channels);
        matching_image = cvCreateImage(cvSize(template_search_size,template_search_size),depth,channels);

        fillImageStack();
        cvWaitKey(0);

    }
    else
    {
        printf("Enter template image filename!\n[usage: ./tracer 'filename']\n");
    }

    //releaseMemory();
    return 0;
}

void releaseMemory()
{
    cvReleaseImage(&src);
    cvReleaseImage(&src_template_fix);
    cvReleaseImage(&target);
    cvReleaseImage(&template_image);
    cvReleaseImage(&matching_image);
    cvReleaseImage(&matching_result);
    cvDestroyWindow("CHOOSE TEMPLATE");
}

void fillImageStack()
{
    for(int i = startImageID; i < maxImageID; i++)
    {
        filename = "";
        filename.append(path.c_str());
        sprintf(filenameBuffer,"%d",i);
        filename.append(filenameBuffer);
        filename.append(".png");
        filenameStack.push_back(filename);
    }
}

void mouseHandlerSingleMode(int event, int x, int y, int flags, void *param)
{
    if(event == CV_EVENT_LBUTTONDOWN)
    {

        xPos = x;
        yPos = y;

        src_template_fix = cvLoadImage(filenameTemplate.c_str(),1);

        cvSetImageROI(src_template_fix, cvRect((xPos - template_size * 0.5),
                                               (yPos - template_size * 0.5),
                                               template_size,
                                               template_size));
        cvCopy(src_template_fix, template_image);

        cvShowImage("TEMPLATE", template_image);

        cvResetImageROI(src_template_fix);

        cvRectangle(src_template_fix,
                    cvPoint(xPos - template_size * 0.5, yPos - template_size * 0.5),
                    cvPoint(xPos + template_size * 0.5, yPos + template_size * 0.5),
                    cvScalar(255, 255, 0, 0),
                    1, 0, 0);
        cvShowImage("CHOOSE TEMPLATE", src_template_fix);

        templateDefined = true;
    }
    else if (event == CV_EVENT_RBUTTONDOWN && templateDefined)
    {
        locateSingleMode(xPos,yPos);
    }
}

void mouseHandlerBatchMode(int event, int x, int y, int flags, void *param)
{

    if(event == CV_EVENT_LBUTTONDOWN)
    {
        xPos = x;
        yPos = y;


        src_template_fix = cvLoadImage(filenameTemplate.c_str(),1);

        cvSetImageROI(src_template_fix, cvRect((xPos - template_size * 0.5),
                                               (yPos - template_size * 0.5),
                                               template_size,
                                               template_size));
        cvCopy(src_template_fix, template_image);

        cvShowImage("TEMPLATE", template_image);

        cvResetImageROI(src_template_fix);

        cvRectangle(src_template_fix,
                    cvPoint(xPos - template_size * 0.5, yPos - template_size * 0.5),
                    cvPoint(xPos + template_size * 0.5, yPos + template_size * 0.5),
                    cvScalar(255, 255, 0, 0),
                    1, 0, 0);
        cvShowImage("CHOOSE TEMPLATE", src_template_fix);

        templateDefined = true;

    }
    else if(event == CV_EVENT_RBUTTONDOWN && templateDefined)
    {
        locateBatchMode(xPos,yPos);
    }
}


void locateBatchMode(int _x, int _y)
{
    // save traced positions in a file
    std::ofstream os;
    os.open("positions.txt");

    printf("processing...\n");

    for(int i = startImageID; i < maxImageID; i++)
    {
        cvReleaseImage(&target);
        target = cvLoadImage(filenameStack[i].c_str(),1);
        cvShowImage("TARGET IMAGE",target);
        cvSetImageROI(target, cvRect((_x - template_search_size * 0.5),
                                               (_y - template_search_size * 0.5),
                                               template_search_size,
                                               template_search_size));
        cvCopy(target, matching_image);
        cvResetImageROI(target);

        cvRectangle(target,
                    cvPoint(_x - template_search_size * 0.5, _y - template_search_size * 0.5),
                    cvPoint(_x + template_search_size * 0.5, _y + template_search_size * 0.5),
                    cvScalar(255, 255, 0, 0),
                    1, 0, 0
                    );

        cvMatchTemplate(matching_image, template_image, matching_result, CV_TM_SQDIFF);
        cvMinMaxLoc(matching_result, &minVal, &maxVal, &minLoc, &maxLoc);

        cvRectangle(target,
                    cv::Point( _x - template_search_size* 0.5 + minLoc.x,
                               _y - template_search_size* 0.5 + minLoc.y),
                    cv::Point( _x - template_search_size* 0.5 + minLoc.x + template_size,
                               _y - template_search_size* 0.5 + minLoc.y + template_size),
                    cvScalar(0,0,255,0),
                    1,0,0
                    );


        cvCircle(target,
                 cv::Point(_x - template_search_size* 0.5 + minLoc.x + template_size * 0.5,
                           _y - template_search_size* 0.5 + minLoc.y + template_size * 0.5),
                 10,
                 cvScalar(0,255,255,0),
                 1,0,0
                 );
        os << i << "\t\t"
           << _x - template_search_size* 0.5 + minLoc.x + template_size * 0.5 << "\t\t"
           << _y - template_search_size* 0.5 + minLoc.y + template_size * 0.5 << "\n";

        cvShowImage("TARGET IMAGE", target);
        cvShowImage("MATCHING RESULT", matching_result);
        cvWaitKey(1);

    }
    os.close();
    printf("done\n");
}

void locateSingleMode(int _x, int _y)
{
    target = cvLoadImage(filenameTarget.c_str(),1);
    cvShowImage("TARGET IMAGE",target);
    cvSetImageROI(target, cvRect((_x - template_search_size * 0.5),
                                           (_y - template_search_size * 0.5),
                                           template_search_size,
                                           template_search_size));
    cvCopy(target, matching_image);
    cvResetImageROI(target);

    cvRectangle(target,
                cvPoint(_x - template_search_size * 0.5, _y - template_search_size * 0.5),
                cvPoint(_x + template_search_size * 0.5, _y + template_search_size * 0.5),
                cvScalar(255, 255, 0, 0),
                1, 0, 0
                );

    cvMatchTemplate(matching_image, template_image, matching_result, CV_TM_SQDIFF);
    cvMinMaxLoc(matching_result, &minVal, &maxVal, &minLoc, &maxLoc);

    cvRectangle(target,
                cv::Point( _x - template_search_size* 0.5 + minLoc.x,
                           _y - template_search_size* 0.5 + minLoc.y),
                cv::Point( _x - template_search_size* 0.5 + minLoc.x + template_size,
                           _y - template_search_size* 0.5 + minLoc.y + template_size),
                cvScalar(0,0,255,0),
                1,0,0
                );
    cvShowImage("TARGET IMAGE", target);
    cvShowImage("MATCHING RESULT", matching_result);
}
