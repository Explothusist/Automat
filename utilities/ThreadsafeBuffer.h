#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_THREADSAFE_BUFFER_

#ifndef AUTOMAT_THREADSAFE_BUFFER_
#define AUTOMAT_THREADSAFE_BUFFER_

namespace atmt {

    // Thread safety for one writer and one reader
    template <typename Type>
    class ThreadsafeBuffer {
        public:
            ThreadsafeBuffer();

            void write(const Type& command);
            Type read();

        private:
            Type m_buffer_1;
            Type m_buffer_2;

            Type* m_write_buffer;
            Type* m_read_buffer;
    };

    template <typename Type>
    ThreadsafeBuffer<Type>::ThreadsafeBuffer():
        m_buffer_1{ },
        m_buffer_2{ },
        m_write_buffer{ &m_buffer_1 },
        m_read_buffer{ &m_buffer_2 }
    {

    };

    template <typename Type>
    void ThreadsafeBuffer<Type>::write(const Type& command) {
        *m_write_buffer = command;
        
        // Stepper_Command* temp = m_write_buffer;
        // m_write_buffer = m_read_buffer;
        // m_read_buffer = temp;
        std::swap(m_read_buffer, m_write_buffer); // Same thing
    };
    template <typename Type>
    Type ThreadsafeBuffer<Type>::read() {
        return *m_read_buffer;
    };

};

#endif

#endif