#ifndef SPEEDTEST_H
#define SPEEDTEST_H

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

#endif
