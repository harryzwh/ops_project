cmake_minimum_required (VERSION 2.8)

set(OPS_SCHED_SRC ops_scheduler.c)

add_library(OPS_SCHED SHARED ${OPS_SCHED_SRC})