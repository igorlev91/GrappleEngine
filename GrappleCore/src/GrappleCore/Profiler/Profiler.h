#pragma once

#include "GrappleCore/Core.h"
#include "GrappleCore/Assert.h"

#include <stdint.h>
#include <chrono>

#ifdef Grapple_RELEASE
    #define Grapple_PROFILING_ENABLED
#endif

#define Grapple_PROFILER_TRACY
// #define Grapple_PROFILER_NATIVE

#include <Tracy.hpp>

namespace Grapple
{
    struct ProfilerScopeTimer;

    class GrappleCORE_API Profiler
    {
    public:
        struct Record
        {
            const char* Name;
            uint64_t StartTime;
            uint64_t EndTime;
        };

        struct RecordsBuffer
        {
            RecordsBuffer(size_t capacity)
                : Records(new Record[capacity]), Size(0), Capacity(capacity) {}

            RecordsBuffer(RecordsBuffer&& other) noexcept
            {
                Records = other.Records;
                Size = other.Size;
                Capacity = other.Capacity;

                other.Records = nullptr;
                other.Size = 0;
                other.Capacity = 0;
            }

            ~RecordsBuffer()
            {
                delete[] Records;
            }

            inline bool IsEmpty() const { return Size == 0; }
            inline bool IsFull() const { return Size == Capacity; }

            Record* Records;
            size_t Capacity;
            size_t Size;
        };

        struct Frame
        {
            inline size_t GetRecordsCount() const { return RecordsCount - FirstRecordIndex; }

            size_t FirstRecordIndex;
            size_t RecordsCount;
        };

        static void BeginFrame();
        static void EndFrame();

        static void StartRecording();
        static void StopRecording();

        static size_t GetRecordsCountPerBuffer();
        static size_t GetRecordsCount();
        static Record& GetRecord(size_t index);

        static bool IsRecording();
        static size_t GetBuffersCount();
        static const RecordsBuffer& GetRecordsBuffer(size_t index);
        static const std::vector<Frame>& GetFrames();

        static void ClearData();
    private:
        static Record* CreateRecord();

        friend struct ProfilerScopeTimer;
    };

    struct GrappleCORE_API ProfilerScopeTimer
    {
    public:
        ProfilerScopeTimer(const char* name);
        ~ProfilerScopeTimer();
    private:
        Profiler::Record* m_Record;
    };

#ifdef Grapple_PROFILING_ENABLED
    #ifdef Grapple_PROFILER_TRACY
        #define Grapple_PROFILE_BEGIN_FRAME(name) FrameMarkStart(name)
        #define Grapple_PROFILE_END_FRAME(name) FrameMarkEnd(name)

        #define Grapple_PROFILE_SCOPE(name) ZoneScopedN(name)
        #define Grapple_PROFILE_FUNCTION() Grapple_PROFILE_SCOPE(__FUNCSIG__)
    #elif defined(Grapple_PROFILER_NATIVE)
        #define Grapple_PROFILE_BEGIN_FRAME(name) Grapple::Profiler::BeginFrame()
        #define Grapple_PROFILE_END_FRAME(name) Grapple::Profiler::EndFrame()

        #define Grapple_PROFILE_TIMER_NAME2(line) ___profileTimer___##line
        #define Grapple_PROFILE_TIMER_NAME(line) Grapple_PROFILE_TIMER_NAME2(line)

        #define Grapple_PROFILE_SCOPE(name) Grapple::ProfilerScopeTimer Grapple_PROFILE_TIMER_NAME(__LINE__) = Grapple::ProfilerScopeTimer(name)
        #define Grapple_PROFILE_FUNCTION() Grapple_PROFILE_SCOPE(__FUNCSIG__)
    #endif
#else
    #define Grapple_PROFILE_BEGIN_FRAME(name)
    #define Grapple_PROFILE_END_FRAME(name)
    #define Grapple_PROFILE_SCOPE(name)
    #define Grapple_PROFILE_FUNCTION()
#endif

}