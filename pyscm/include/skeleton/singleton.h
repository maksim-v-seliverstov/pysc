#ifndef SKELETON_SINGLETON_H_
#define SKELETON_SINGLETON_H_

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

template <class T>
class Singleton: public T {
  public:
    static Singleton* Instance();
    static Singleton* InstanceAggregated();

    void FreeInstance();

  private:
    static Singleton* m_Self;
    static int m_RefCount;

    static Singleton* instance(bool aggregated);

    Singleton();
    ~Singleton();

    Singleton(const Singleton&);
    Singleton& operator = (const Singleton&);
};

template <class T> Singleton<T>* Singleton<T>::m_Self = nullptr;
template <class T> int Singleton<T>::m_RefCount = 0;

template <class T>
Singleton<T>* Singleton<T>::Instance() {
    return instance(false);
}

template <class T>
Singleton<T>* Singleton<T>::InstanceAggregated() {
    return instance(true);
}

template <class T>
Singleton<T>* Singleton<T>::instance(bool aggregated) {
    if (!m_Self) {
        m_Self = new Singleton();
    }
    if (!aggregated) {
        ++m_RefCount;
    }
    return m_Self;
}

template <class T>
void Singleton<T>::FreeInstance() {
    --m_RefCount;
    if (!m_RefCount) {
        delete this;
        m_Self = nullptr;
    }
}

template <class T> Singleton<T>::Singleton() {}
template <class T> Singleton<T>::~Singleton() {}

template <class T>
void intrusive_ptr_add_ref(T* p) {
    T::Instance();
}

template <class T>
void intrusive_ptr_release(T* p) {
    p->FreeInstance();
}

#endif  // SKELETON_SINGLETON_H_
