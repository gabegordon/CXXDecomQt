#include "catch/catch.hpp"
#include "backend.hpp"

class BackendTest
{
public:
    BackendTest() :
        backend(nullptr)
    {}
    ~BackendTest() {}
    BackEnd backend;
};
TEST_CASE_METHOD(BackendTest, "Backend Checks")
{
}
