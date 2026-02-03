#include "speedtest.h"
#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include <time.h>

#pragma comment(lib, "winhttp.lib")

#define TEST_SERVER L"speed.cloudflare.com"
#define DOWNLOAD_PATH L"/__down?bytes=10485760"
#define PING_PATH L"/"
#define UPLOAD_SIZE (1024 * 1024)

int test_ping(double *result_ms) {
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    DWORD start_time, end_time;
    int status = -1;

    hSession = WinHttpOpen(L"Speedtest/1.0",
                          WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                          WINHTTP_NO_PROXY_NAME,
                          WINHTTP_NO_PROXY_BYPASS, 0);
    
    if (!hSession) goto cleanup;

    hConnect = WinHttpConnect(hSession, TEST_SERVER, INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) goto cleanup;

    hRequest = WinHttpOpenRequest(hConnect, L"HEAD", PING_PATH,
                                 NULL, WINHTTP_NO_REFERER,
                                 WINHTTP_DEFAULT_ACCEPT_TYPES,
                                 WINHTTP_FLAG_SECURE);
    
    if (!hRequest) goto cleanup;

    start_time = GetTickCount();
    
    if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                          WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(hRequest, NULL)) {
        
        end_time = GetTickCount();
        *result_ms = (double)(end_time - start_time);
        status = 0;
    }

cleanup:
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
    
    return status;
}

int test_download(double *result_mbps) {
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    DWORD bytes_read;
    DWORD total_bytes = 0;
    DWORD start_time, end_time;
    BYTE buffer[8192];
    int status = -1;

    hSession = WinHttpOpen(L"Speedtest/1.0",
                          WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                          WINHTTP_NO_PROXY_NAME,
                          WINHTTP_NO_PROXY_BYPASS, 0);
    
    if (!hSession) goto cleanup;

    hConnect = WinHttpConnect(hSession, TEST_SERVER, INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) goto cleanup;

    hRequest = WinHttpOpenRequest(hConnect, L"GET", DOWNLOAD_PATH,
                                 NULL, WINHTTP_NO_REFERER,
                                 WINHTTP_DEFAULT_ACCEPT_TYPES,
                                 WINHTTP_FLAG_SECURE);
    
    if (!hRequest) goto cleanup;

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                           WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        goto cleanup;
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        goto cleanup;
    }

    start_time = GetTickCount();
    
    while (WinHttpReadData(hRequest, buffer, sizeof(buffer), &bytes_read) && bytes_read > 0) {
        total_bytes += bytes_read;
    }
    
    end_time = GetTickCount();
    
    if (total_bytes > 0) {
        double elapsed_seconds = (double)(end_time - start_time) / 1000.0;
        if (elapsed_seconds > 0) {
            double bits_per_second = (total_bytes * 8.0) / elapsed_seconds;
            *result_mbps = bits_per_second / 1000000.0;
            status = 0;
        }
    }

cleanup:
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
    
    return status;
}

int test_upload(double *result_mbps) {
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    DWORD start_time, end_time;
    BYTE *upload_data = NULL;
    int status = -1;

    upload_data = (BYTE*)malloc(UPLOAD_SIZE);
    if (!upload_data) goto cleanup;
    
    for (int i = 0; i < UPLOAD_SIZE; i++) {
        upload_data[i] = (BYTE)(rand() % 256);
    }

    hSession = WinHttpOpen(L"Speedtest/1.0",
                          WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                          WINHTTP_NO_PROXY_NAME,
                          WINHTTP_NO_PROXY_BYPASS, 0);
    
    if (!hSession) goto cleanup;

    hConnect = WinHttpConnect(hSession, TEST_SERVER, INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) goto cleanup;

    hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/__up",
                                 NULL, WINHTTP_NO_REFERER,
                                 WINHTTP_DEFAULT_ACCEPT_TYPES,
                                 WINHTTP_FLAG_SECURE);
    
    if (!hRequest) goto cleanup;

    start_time = GetTickCount();
    
    if (WinHttpSendRequest(hRequest, L"Content-Type: application/octet-stream\r\n", -1,
                          upload_data, UPLOAD_SIZE, UPLOAD_SIZE, 0) &&
        WinHttpReceiveResponse(hRequest, NULL)) {
        
        end_time = GetTickCount();
        
        double elapsed_seconds = (double)(end_time - start_time) / 1000.0;
        if (elapsed_seconds > 0) {
            double bits_per_second = (UPLOAD_SIZE * 8.0) / elapsed_seconds;
            *result_mbps = bits_per_second / 1000000.0;
            status = 0;
        }
    }

cleanup:
    if (upload_data) free(upload_data);
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
    
    return status;
}
