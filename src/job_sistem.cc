#include "..\include\job_sistem.h"

JobSystem::JobSystem()
{

    threads = std::thread::hardware_concurrency();

}
