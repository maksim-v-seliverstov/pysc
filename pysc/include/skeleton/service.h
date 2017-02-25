#ifndef WIN_SERVICE_SCM_SERVICE_H_
#define WIN_SERVICE_SCM_SERVICE_H_

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <windows.h>
#include <tchar.h>

#include <boost/bind.hpp>
#include <boost/intrusive_ptr.hpp>

#include <cstdint>
#include <cassert>

#include <string>

#include "skeleton/guard.h"
#include "skeleton/singleton.h"

namespace service {
    typedef std::basic_string<TCHAR> tstring;
    const tstring ENV_FOR_SERVICE_NAME = TEXT("8198CE51-0D47-4FF9-9CDE-C531D466A7F7");

    uint32_t service_wait() {
        DWORD szBuffer = GetEnvironmentVariable(ENV_FOR_SERVICE_NAME.c_str(), nullptr, 0);
        if (!szBuffer) {
            return GetLastError();
        }
        std::shared_ptr<TCHAR> service_name(new TCHAR[szBuffer]);
        if (!GetEnvironmentVariable(ENV_FOR_SERVICE_NAME.c_str(), service_name.get(), szBuffer)) {
            return GetLastError();
        }

        auto deinit = boost::bind(CloseHandle, _1);
        auto open_event = boost::bind(OpenEvent, EVENT_ALL_ACCESS, FALSE, service_name.get());
        Guard<decltype(open_event), decltype(deinit), HANDLE> hEvent(open_event, deinit);
        if (!hEvent()) {
            return GetLastError();
        }

        WaitForSingleObject(hEvent.get(), INFINITE);

        return 0;
    }
}  // namespace service

#endif  // WIN_SERVICE_SCM_SERVICE_H_
