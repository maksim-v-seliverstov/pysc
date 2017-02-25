#ifndef SKELETON_GUARD_H_
#define SKELETON_GUARD_H_

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

namespace service {

template <class FUNC_INIT_T, class FUNC_DEINIT_T, class RETURN_T> class Guard {
  public:
    Guard(FUNC_INIT_T init, FUNC_DEINIT_T deinit)
        : m_Init(init)
        , m_Deinit(deinit) {}
    ~Guard() {
        if (m_Result) {
            m_Deinit(m_Result);
        }
    }
    RETURN_T operator () () { return m_Result = m_Init(); }
    RETURN_T get() const { return m_Result; }
  private:
    FUNC_INIT_T m_Init;
    FUNC_DEINIT_T m_Deinit;
    RETURN_T m_Result;
};
}  // namespace service

#endif  // SKELETON_GUARD_H_
