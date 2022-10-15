#include "processor.h"

#include <iostream>

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  return (double)LinuxParser::ActiveJiffies() / (double)LinuxParser::Jiffies();
}