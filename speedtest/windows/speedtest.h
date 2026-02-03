#ifndef SPEEDTEST_H
#define SPEEDTEST_H

#include <windows.h>

typedef struct {
    double ping_ms;
    double download_mbps;
    double upload_mbps;
    int status;
    char error_msg[256];
} SpeedTestResult;

int test_ping(double *result_ms);
int test_download(double *result_mbps);
int test_upload(double *result_mbps);
void update_ui_status(HWND hwnd, const char *status);
void update_ui_results(HWND hwnd, SpeedTestResult *result);

#endif
