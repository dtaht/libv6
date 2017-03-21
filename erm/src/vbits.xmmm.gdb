define bm
break main
end

define vbits
  print /t $xmm0.uint128
  print /t $xmm1.uint128
  print /t $xmm2.uint128
  print /t $xmm3.uint128
  print /t $xmm4.uint128
  print /t $xmm5.uint128
  print /t $xmm6.uint128
  print /t $xmm7.uint128
  print /t $xmm8.uint128
  print /t $xmm9.uint128
  print /t $xmm10.uint128
  print /t $xmm11.uint128
  print /t $xmm12.uint128
  print /t $xmm13.uint128
  print /t $xmm14.uint128
  print /t $xmm15.uint128
end
document vbits
Show the vector regs as bits
end
