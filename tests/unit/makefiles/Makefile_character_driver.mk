COMPONENT_NAME=character_driver

SRC_FILES = \
  $(UNITTEST_ROOT)/fakes/fake_event_loop.cpp \
  $(PROJECT_SRC_DIR)/device/error_status.cpp 

TEST_SRC_FILES = \
  $(UNITTEST_SRC_DIR)/driver/test_character_driver.cpp

include $(CPPUTEST_MAKEFILE_INFRA)
