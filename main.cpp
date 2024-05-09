#include <QCoreApplication>

#include <windows.h>
#include <iostream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

void SetServiceStartMode(DWORD dwProcessId,int config)
{
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCManager)
    {
        cerr << "Failed to open service manager" << endl;
        return;
    }

    // Open the service associated with the process ID
    WCHAR szProcessId[20];
    wsprintf(szProcessId, L"%d", dwProcessId);
    SC_HANDLE hService = OpenService(hSCManager, szProcessId, SERVICE_CHANGE_CONFIG);
    if (!hService)
    {
        cerr << "Failed to open service for process ID " << dwProcessId << endl;
        CloseServiceHandle(hSCManager);
        return;
    }

    // Change the start type to Automatic
    BOOL bSuccess = ChangeServiceConfig(hService,
        SERVICE_NO_CHANGE,
        config,
        SERVICE_NO_CHANGE,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);

    if (!bSuccess)
    {
        cerr << "Failed to change service start type" << endl;
    }
    else
    {
        cout << "Service start type has been set to Automatic" << endl;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SC_HANDLE scm_handle = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (scm_handle == NULL) {
        std::cout << "Unable to open Service Control Manager." << std::endl;
        return 1;
    }

    DWORD services_count;
    DWORD resume_handle = 0;
    DWORD buffer_size = 0;
    EnumServicesStatusEx(scm_handle, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_ACTIVE,
                          NULL, 0, &buffer_size, &services_count, &resume_handle, NULL);

    LPENUM_SERVICE_STATUS_PROCESS services_info = new ENUM_SERVICE_STATUS_PROCESS[buffer_size];
    if (!EnumServicesStatusEx(scm_handle, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_ACTIVE,
                          reinterpret_cast<LPBYTE>(services_info), buffer_size, &buffer_size,
                          &services_count, &resume_handle, NULL)) {
        std::cout << "Unable to enumerate services." << std::endl;
        CloseServiceHandle(scm_handle);
        delete[] services_info;
        return 1;
    }

//SERVICE_AUTO_START
    for (DWORD i = 0; i < services_count; ++i) {
std::cout<<i<<"  ";
        SetServiceStartMode(services_info[i].ServiceStatusProcess.dwProcessId,SERVICE_DEMAND_START);
    std::cout<<std::endl;
    }

    delete[] services_info;
    CloseServiceHandle(scm_handle);

    return 0;
}
