#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <string>

using namespace std;

#define FADE_IN 1
#define FADE_OUT -1

int COL = 1200;
int ROW = 800;
int delay = 15;
const string outputName = "output/video.mp4";
static cv::VideoWriter outputVideo;
cv::Size showSize = cv::Size(COL, ROW);
cv::Point p[1][5] = {
    cv::Point(250, 600),
    cv::Point(950, 600),
    cv::Point(950, 700),
    cv::Point(250, 700),
    cv::Point(250, 600)
};

const cv::Point* pp[] = { p[0] };
int npt[] = { 5 };

int TextFont = cv::FONT_HERSHEY_COMPLEX;
cv::Scalar WHITE(CV_RGB(255, 255, 255));
cv::Scalar BLACK(CV_RGB(0, 0, 0));

void fade(cv::Mat src, int flag, int step_size = 5);
cv::Mat colorit();
cv::Mat film();
cv::Mat Resize(cv::Mat src, bool central);
cv::Mat Text(cv::Mat src, string str, cv::Point pos, double width, cv::Scalar color);
void output(cv::VideoWriter video, cv::Mat frame, int num = 4);
void attenuation(cv::Mat scr, cv::Mat des);
void end();

int main(void)
{
    outputVideo.open(outputName, 0x7634706d, 50, showSize, true);
    if (!outputVideo.isOpened())
    {
        cout << "Could not open the output video to write: " << outputName << endl;
        return -1;
    }

    /* Pictures Input and Process */
    cv::Mat img, resizedImg, img2;
    img = cv::imread("input/photo.jpg");
    resizedImg = Resize(img, false);
    fade(resizedImg, FADE_IN);
    Text(resizedImg, "Lu Hanzhen\n3200105841", cv::Point(0.5 * COL, 0.5 * ROW), 2, WHITE);
    fade(resizedImg, FADE_OUT);
    img = colorit();
    img2 = cv::imread("input/1.png");
    resizedImg = Resize(img2, true);
    cv::fillPoly(resizedImg, pp, npt, 1, WHITE);
    attenuation(img, resizedImg);
    img = film();
    fade(img, FADE_OUT);
    end();
    /* Release all */
    cv::destroyAllWindows();
    cout << "Success!" << endl;

    return 0;
}

void fade(cv::Mat src, int flag, int step_size)
{
    cv::Mat fadeImg;
    
    int step = 255 / step_size;
    for (int i = 0; i < step; i++)
    {
        if (flag == FADE_IN)
            cv::subtract(src, (255 - i * step_size), fadeImg);//third parameter = first parameter - second parameter
        else if (flag == FADE_OUT)
            cv::subtract(src, (i * step_size), fadeImg);
        outputVideo << fadeImg;
    }
}

cv::Mat Resize(cv::Mat src, bool central) {
    cv::Mat result, warp_matrix;
    double ration_x, ration_y;
    ration_x = (double)COL / src.cols;
    ration_y = (double)ROW / src.rows;
    if (ration_x > ration_y) cv::resize(src, result, cv::Size(src.cols * ration_y, src.rows * ration_y), 0, 0);
    else cv::resize(src, result, cv::Size(src.cols * ration_x, src.rows * ration_x), 0, 0);
    if (central) {
        warp_matrix = (cv::Mat_<float>(2, 3) <<
            cos(0), -sin(0), (COL - result.cols) / 2,
            sin(0), cos(0), (ROW - result.rows) / 2);
    }
    else {
        warp_matrix = (cv::Mat_<float>(2, 3) <<
            cos(0), -sin(0), 0,
            sin(0), cos(0), 0);
    }
    cv::warpAffine(result, result, warp_matrix, showSize);
    return result;
}

cv::Mat Text(cv::Mat src, string str, cv::Point pos, double width, cv::Scalar color) {
    int len = str.length();
    vector<string> list;
    string* temp = new string("");
    for (int i = 0; i < len; i++) {//split the string
        if (str[i] == '\n') {
            list.push_back(*temp);
            temp = new string("");
        }
        else {
            (*temp) += str[i];
        }
    }
    list.push_back(*temp);
    for (int j = 0; j < list.size(); j++) {//print each substring and change the text position
        for (int i = 0; i < len; i++) {
            cv::putText(src, list[j].substr(0, i), pos, TextFont, width, color);
            output(outputVideo, src);
        }
        pos.y += 30 * width;
    }
    return src;
}

void output(cv::VideoWriter video, cv::Mat frame, int num) {
    for (int i = 0; i < num; i++) {
        video << frame;
    }
}

cv::Mat colorit() {
    cv::Mat img(showSize, 16);
    cv::Point p1, p2;
    for (int i = 0; i < 40; i++) {
        //draw rectangle with same center, each rectangle has less height and length than former
        cv::rectangle(img, cv::Rect(i * 8, i * 8, COL - i * 16, ROW - i * 16), cv::Scalar(rand() % 255, rand() % 255, rand() % 255), 8);
        output(outputVideo, img);
    }
    for (int i = 0; i < 20; i++) {
        //draw rectangle with same center, each rectangle has less length than former
        cv::rectangle(img, cv::Rect(312 + i * 8, 312, COL - 624 - i * 16, ROW - 624), cv::Scalar(rand() % 255, rand() % 255, rand() % 255), 8);
        output(outputVideo, img);
    }
    for (int i = 0; i < 40; i++) {
        cv::circle(img, cv::Point(COL / 2, ROW / 2), i * 4, cv::Scalar(rand() % 255, rand() % 255, rand() % 255), 2);
        output(outputVideo, img);
    }
    for (int i = 0; i < 150; i++) {
        //random select two points
        p1.x = rand() % COL;
        p1.y = rand() % ROW;
        p2.x = rand() % COL;
        p2.y = rand() % ROW;
        cv::line(img, p1, p2, cv::Scalar(rand() % 255, rand() % 255, rand() % 255), 3);//draw a line between them
        outputVideo << img;
    }
    return img;
}

cv::Mat film() {
    cv::Mat shot, temp;

    shot = cv::imread("input/1.png");
    shot = Resize(shot, true);
    cv::fillPoly(shot, pp, npt, 1, WHITE);
    Text(shot, "I need a job\n", cv::Point(450, 650), 1, BLACK);
    shot = cv::imread("input/3.png");
    shot = Resize(shot, true);
    cv::fillPoly(shot, pp, npt, 1, WHITE);
    Text(shot, "You need experience first\n", cv::Point(350, 650), 1, BLACK);
    shot = cv::imread("input/4.png");
    shot = Resize(shot, true);
    cv::fillPoly(shot, pp, npt, 1, WHITE);
    Text(shot, "But I need a job\nto gain experience!\n", cv::Point(400, 650), 1, BLACK);
    shot = cv::imread("input/5.png");
    shot = Resize(shot, true);
    cv::fillPoly(shot, pp, npt, 1, WHITE);
    Text(shot, "Then go find a job!\n", cv::Point(400, 650), 1, BLACK);
    shot = cv::imread("input/6.png");
    shot = Resize(shot, true);
    cv::fillPoly(shot, pp, npt, 1, WHITE);
    Text(shot, "That's exactly why I'm here!\n", cv::Point(300, 650), 1, BLACK);
    shot = cv::imread("input/7.png");
    shot = Resize(shot, true);
    cv::fillPoly(shot, pp, npt, 1, WHITE);
    return Text(shot, "But you don't have any experience!\n", cv::Point(250, 650), 1, BLACK);
}

void attenuation(cv::Mat src, cv::Mat des)
{
    int row = src.rows;
    int col = src.cols;
    for (int c = 0; c < 200; c++) {
        for (int i = 0; i < row; i++)
        {
            uchar* s = src.ptr<uchar>(i);
            uchar* d = des.ptr<uchar>(i);
            for (int j = 0; j < col; j++)
            {
                if (s[3 * j + 2] < d[3 * j + 2]) s[3 * j + 2]++;
                else if (s[3 * j + 2] > d[3 * j + 2]) s[3 * j + 2]--;
                if (s[3 * j + 1] < d[3 * j + 1]) s[3 * j + 1]++;
                else if (s[3 * j + 1] > d[3 * j + 1]) s[3 * j + 1]--;
                if (s[3 * j] < d[3 * j]) s[3 * j]++;
                else if (s[3 * j] > d[3 * j]) s[3 * j]--;
            }
        }
        outputVideo << src;
    } 
}

void end() {
    cv::Mat img(showSize, 16);
    Text(img, "The End\n Thanks", cv::Point(0.3 * COL, 0.4 * ROW), 3, WHITE);
}