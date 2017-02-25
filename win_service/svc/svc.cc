#include <windows.h>

#include <boost/intrusive_ptr.hpp>

#include "skeleton/singleton.h"
#include "skeleton/service.h"

void WINAPI SvcMain(DWORD, LPTSTR*);
void WINAPI SvcCtrlHandler(DWORD);

static void ReportSvcStatus(DWORD, DWORD, DWORD);

struct SERVICE_PARAMETERS {
    TCHAR** m_Argv;
    HANDLE m_ScriptStopEvent;
    SERVICE_STATUS_HANDLE m_SvcStatusHandle;
    SERVICE_STATUS m_SvcStatus;
    DWORD m_WaitHintForStart;
    DWORD m_WaitHintForStop;
};

int _tmain(int argc, TCHAR *argv[], TCHAR *envp[]) {
    boost::intrusive_ptr<Singleton<SERVICE_PARAMETERS>> service_parameters(Singleton<SERVICE_PARAMETERS>::InstanceAggregated());
    if (argc > 1) {
        service_parameters->m_Argv = &argv[1];
        service_parameters->m_ScriptStopEvent = nullptr;
        service_parameters->m_WaitHintForStart = 15000;
        service_parameters->m_WaitHintForStop = 30000;
    } else {
        return -1;
    }

    SERVICE_TABLE_ENTRY DispatchTable[] = {
        {TEXT(""), static_cast<LPSERVICE_MAIN_FUNCTION>(SvcMain)},
        {nullptr, nullptr}
    };

    if (!StartServiceCtrlDispatcher(DispatchTable)) {
        return GetLastError();
    }

    return 0;
}

void WINAPI SvcMain(DWORD dwArgc, LPTSTR* lpszArgv) {
    TCHAR* service_name = lpszArgv[0];

    boost::intrusive_ptr<Singleton<SERVICE_PARAMETERS>> service_parameters(Singleton<SERVICE_PARAMETERS>::Instance());
    service_parameters->m_SvcStatusHandle = RegisterServiceCtrlHandler(service_name, SvcCtrlHandler);
    if (!service_parameters->m_SvcStatusHandle) {
        return;
    }

    service_parameters->m_SvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    service_parameters->m_SvcStatus.dwServiceSpecificExitCode = 0;

    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, service_parameters->m_WaitHintForStart);

    SECURITY_DESCRIPTOR sd;
    SECURITY_ATTRIBUTES sa;

    ZeroMemory(&sd, sizeof(sd));
    ZeroMemory(&sa, sizeof(sa));

    if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) {
        return;
    }

    if (!SetSecurityDescriptorDacl(&sd, TRUE, nullptr, FALSE)) {
        return;
    }

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = &sd;
    sa.bInheritHandle = FALSE;

    service_parameters->m_ScriptStopEvent = CreateEvent(&sa, TRUE, FALSE, lpszArgv[0]);
    if (!service_parameters->m_ScriptStopEvent) {
        return;
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    std::basic_string<TCHAR> command_line;
    for (TCHAR** p = service_parameters->m_Argv; *p != nullptr; ++p) {
        command_line += *p;
        command_line += TEXT(" ");
    }

    SetEnvironmentVariable(service::ENV_FOR_SERVICE_NAME.c_str(), service_name);

    if (!CreateProcess(nullptr, const_cast<LPTSTR>(command_line.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        return;
    }

    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(service_parameters->m_ScriptStopEvent);

    ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

void WINAPI SvcCtrlHandler(DWORD dwCtrl) {
    switch (dwCtrl) {
    case SERVICE_CONTROL_STOP:
        boost::intrusive_ptr<Singleton<SERVICE_PARAMETERS>> service_parameters(Singleton<SERVICE_PARAMETERS>::Instance());

        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, service_parameters->m_WaitHintForStop);
        if (!SetEvent(service_parameters->m_ScriptStopEvent)) {
            return;
        }
        break;
    }
}

void ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint) {
    static DWORD dwCheckPoint = 1;

    boost::intrusive_ptr<Singleton<SERVICE_PARAMETERS>> service_parameters(Singleton<SERVICE_PARAMETERS>::Instance());

    service_parameters->m_SvcStatus.dwCurrentState = dwCurrentState;
    service_parameters->m_SvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    service_parameters->m_SvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING) {
        service_parameters->m_SvcStatus.dwControlsAccepted = 0;
    } else {
        service_parameters->m_SvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    }

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED)) {
        service_parameters->m_SvcStatus.dwCheckPoint = 0;
    } else {
        service_parameters->m_SvcStatus.dwCheckPoint = dwCheckPoint++;
    }

    SetServiceStatus(service_parameters->m_SvcStatusHandle, &service_parameters->m_SvcStatus);
}
