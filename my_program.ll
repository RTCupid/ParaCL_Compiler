; ModuleID = 'frontend/tests/end_to_end/test_butterworth_filter/butterworth_bandwidth.txt'
source_filename = "frontend/tests/end_to_end/test_butterworth_filter/butterworth_bandwidth.txt"
target triple = "x86_64-pc-linux-gnu"

@print_fmt = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: nofree nounwind
define noundef i32 @main() local_unnamed_addr #0 {
entry:
  %printfcall = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @print_fmt, i32 1591)
  ret i32 0
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define range(i32 0, 1000000001) i32 @func_0(i32 %0, i32 %1, i32 %2) local_unnamed_addr #1 {
entry:
  %letmp = icmp slt i32 %2, 1
  br i1 %letmp, label %common.ret, label %ifcont

common.ret:                                       ; preds = %ifcont, %entry, %ifcont11
  %common.ret.op = phi i32 [ %divtmp14, %ifcont11 ], [ 0, %entry ], [ 0, %ifcont ]
  ret i32 %common.ret.op

ifcont:                                           ; preds = %entry
  %multmp = mul i32 %0, 6284
  %multmp5 = mul i32 %multmp, %1
  %letmp7 = icmp slt i32 %multmp5, 1000
  br i1 %letmp7, label %common.ret, label %ifcont11

ifcont11:                                         ; preds = %ifcont
  %divtmp16 = udiv i32 %multmp5, 1000
  %divtmp14 = udiv i32 1000000000, %divtmp16
  br label %common.ret
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

attributes #0 = { nofree nounwind }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
