#include "speedtest.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define TEST_URL "https://speed.cloudflare.com"
#define DOWNLOAD_URL "https://speed.cloudflare.com/__down?bytes=10485760"
#define UPLOAD_URL "https://speed.cloudflare.com/__up"
#define UPLOAD_SIZE (1024 * 1024)

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    size_t *total = (size_t *)userp;
    *total += realsize;
    return realsize;
}

static size_t read_callback(char *buffer, size_t size, size_t nitems, void *userp) {
    size_t *remaining = (size_t *)userp;
    size_t to_send = size * nitems;
    
    if (*remaining == 0) return 0;
    
    if (to_send > *remaining) {
        to_send = *remaining;
    }
    
    memset(buffer, 'A', to_send);
    *remaining -= to_send;
    return to_send;
}

static double get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
}

int test_ping(double *result_ms) {
    CURL *curl;
    CURLcode res;
    double start_time, end_time;
    
    curl = curl_easy_init();
    if (!curl) return -1;
    
    curl_easy_setopt(curl, CURLOPT_URL, TEST_URL);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    
    start_time = get_time_ms();
    res = curl_easy_perform(curl);
    end_time = get_time_ms();
    
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        return -1;
    }
    
    *result_ms = end_time - start_time;
    return 0;
}

int test_download(double *result_mbps) {
    CURL *curl;
    CURLcode res;
    size_t total_bytes = 0;
    double start_time, end_time;
    
    curl = curl_easy_init();
    if (!curl) return -1;
    
    curl_easy_setopt(curl, CURLOPT_URL, DOWNLOAD_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &total_bytes);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    
    start_time = get_time_ms();
    res = curl_easy_perform(curl);
    end_time = get_time_ms();
    
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK || total_bytes == 0) {
        return -1;
    }
    
    double elapsed_seconds = (end_time - start_time) / 1000.0;
    if (elapsed_seconds > 0) {
        double bits_per_second = (total_bytes * 8.0) / elapsed_seconds;
        *result_mbps = bits_per_second / 1000000.0;
        return 0;
    }
    
    return -1;
}

int test_upload(double *result_mbps) {
    CURL *curl;
    CURLcode res;
    size_t remaining = UPLOAD_SIZE;
    double start_time, end_time;
    
    curl = curl_easy_init();
    if (!curl) return -1;
    
    curl_easy_setopt(curl, CURLOPT_URL, UPLOAD_URL);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &remaining);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)UPLOAD_SIZE);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    
    start_time = get_time_ms();
    res = curl_easy_perform(curl);
    end_time = get_time_ms();
    
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        return -1;
    }
    
    double elapsed_seconds = (end_time - start_time) / 1000.0;
    if (elapsed_seconds > 0) {
        double bits_per_second = (UPLOAD_SIZE * 8.0) / elapsed_seconds;
        *result_mbps = bits_per_second / 1000000.0;
        return 0;
    }
    
    return -1;
}
