; ModuleID = 'claire'
source_filename = "../../examples/hello_world.clr"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@0 = private unnamed_addr constant [13 x i8] c"Hello world!\00", align 1

define i32 @main() {
entry:
  %calltmp = call i32 @puts(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @0, i32 0, i32 0))
  ret i32 0
}

declare i32 @puts(i8*)

