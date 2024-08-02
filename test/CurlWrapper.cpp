// Let the test here for now. I will integrate this test on another PR

#include <gtest/gtest.h>

#include <CurlWrapper.hpp>

using namespace LokalSo;

TEST(CurlWrapper, CreateCurlWrapper) {
    CurlWrapper curl{};
}


TEST(CurlWrapper, CreateWithProperties) {
    CurlWrapper curl{};
    curl.addHeader("Content-Type", "application/json");
    curl.addHeader("Signature", "JustASignature");
    
    // By default GET
    // curl.setMethod("GET")

    curl.setUserAgent("UserAgent");
    curl.setURL("https://google.com");

    ASSERT_EQ(curl.getReqHeader().getValue("Content-Type") , "application/json");
    ASSERT_EQ(curl.getReqHeader().getValue("Signature") , "JustASignature");


    ASSERT_NO_THROW(curl.execute());

    ASSERT_TRUE(curl.getReqHeader().getHeader().empty());
    ASSERT_TRUE(!curl.getResBody().empty());
    ASSERT_TRUE(!curl.getResHeader().getHeader().empty());
    

}

TEST(CurlWrapper, FailToExecute) {
    CurlWrapper curl{};

    curl.setURL("https://127.0.0.1:8080");

    ASSERT_THROW(curl.execute(), std::runtime_error);
}