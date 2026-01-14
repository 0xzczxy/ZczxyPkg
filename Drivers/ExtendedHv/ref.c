uint64_t sub_fffff8000035cb70(uint128_t** arg1, int32_t arg2, int32_t arg3, int512_t arg4 @ zmm1, int512_t arg5 @ zmm2)
{
  void var_108;
  int64_t rax_1 = __security_cookie ^ &var_108;
  TEB* gsbase;
  struct _EXCEPTION_REGISTRATION_RECORD* ExceptionList_1 =
    gsbase->NtTib.ExceptionList;
  bool cond:0 = !data_fffff80000122730;
  int32_t var_b0 = 0;
  int128_t var_a8;
  __builtin_memset(&var_a8, 0, 0x40);
  char var_d8 = 0;
  
  if (!cond:0)
  {
    int32_t temp0_1;
    int32_t temp1_1;
    temp0_1 = __rdmsr(0x34);
    *(int64_t*)((char*)ExceptionList_1 + 0x1a0) =
      (uint64_t)temp0_1 | (uint64_t)temp1_1 << 0x20;
  }
  
  void* r13 = arg1[-0x160];
  char var_c8 = 0;
  int32_t* r14 = (char*)r13 + 0x13c0;
  
  if (data_fffff80000023e88 == 3)
  {
    void* rax_4 = *(int64_t*)((char*)ExceptionList_1 + 0x480);
    *(uint8_t*)((char*)rax_4 + 0xc4) |= 1;
  }
  
  int64_t r9;
  int0_t tsc;
  
  if (!(data_fffff800000af0b8 & 8))
    r9 = sub_fffff80000256fe0(ExceptionList_1);
  else
  {
    int32_t temp0_2;
    int32_t temp1_2;
    temp0_2 = _rdtsc(tsc);
    int64_t rax_8;
    int64_t rdx_4;
    rdx_4 = HIGHQ(*(int64_t*)((char*)ExceptionList_1 + 0x28)
      * ((uint64_t)temp0_2 | (uint64_t)temp1_2 << 0x20));
    rax_8 = LOWQ(*(int64_t*)((char*)ExceptionList_1 + 0x28)
      * ((uint64_t)temp0_2 | (uint64_t)temp1_2 << 0x20));
    r9 = rdx_4;
  }
  
  void* r8 = *(int64_t*)((char*)ExceptionList_1 + 0x2e9c0);
  int64_t r10_1 = r9 - *(uint64_t*)((char*)r8 + 8);
  int32_t temp0_3;
  int32_t temp1_3;
  temp0_3 = _rdtsc(tsc);
  int64_t rcx_2 = *(uint64_t*)((char*)r8 + 0xa8);
  int64_t rdx_8 = ((uint64_t)temp1_3 << 0x20 | (uint64_t)temp0_3)
    + *(int64_t*)((char*)ExceptionList_1 + 0x70);
  *(uint64_t*)((char*)r8 + 8) = r9;
  void* rax_11 = *(int64_t*)((char*)ExceptionList_1 + 0x2e9c0);
  *(uint64_t*)((char*)rax_11 + 0x10) += r10_1;
  *(uint64_t*)(*(int64_t*)((char*)ExceptionList_1 + 0x2e9c0) + 0xa8) = rdx_8;
  void* rax_13 = *(int64_t*)((char*)ExceptionList_1 + 0x2e9c0);
  *(uint64_t*)((char*)rax_13 + 0xb0) += rdx_8 - rcx_2;
  int64_t rdx_10 = *(int64_t*)((char*)ExceptionList_1 + 0x58);
  void* rax_14 = *(int64_t*)((char*)ExceptionList_1 + 0x2e9c0);
  
  if (rdx_10 >= r10_1)
    rdx_10 = r10_1;
  
  *(uint64_t*)((char*)rax_14 + 0x18) += rdx_10;
  int64_t* r8_1 = *(int64_t*)((char*)ExceptionList_1 + 0x38);
  int64_t rax_15 = r8_1[0x4a7];
  r8_1[0x1e] += rdx_10;
  int64_t rcx_3 = (int64_t)r8_1[0x4f];
  *(uint64_t*)(rax_15 + (rcx_3 << 3) + 8) += rdx_10;
  void* rax_16 = *(uint64_t*)r8_1;
  
  if (rax_16)
  {
    void* rax_17 = *(uint64_t*)((char*)rax_16 + 0x3c0);
    
    if (*(uint8_t*)((char*)rax_17 + 0x14))
    {
      *(uint64_t*)((char*)rax_17 + 0x1270) += r10_1;
      rcx_3 = *(uint64_t*)(*(uint64_t*)r8_1 + 0x3c0);
      *(uint64_t*)(rcx_3 + 0x1278) += r10_1;
    }
  }
  
  *(int64_t*)((char*)ExceptionList_1 + 0x60) = r9 + 0x3e8;
  *(uint32_t*)(*(int64_t*)((char*)ExceptionList_1 + 0x38) + 0x278) = 0x17;
  
  if (data_fffff800000af0b4)
  {
    uint64_t* rcx_4 = *(uint64_t*)((char*)r14 + 0x188);
    
    if (*(uint32_t*)((char*)rcx_4 + 0x6c) == 3 && rcx_4[0xf] == arg2)
    {
      sub_fffff800003358e0(rcx_4, *(uint64_t*)((char*)r14 + 0x158) + 8);
      void* rax_21 = *(uint64_t*)((char*)r14 + 0x158);
      *(uint64_t*)((char*)rax_21 + 8) |= 1;
      *(uint64_t*)(*(uint64_t*)((char*)r14 + 0x188) + 0x18) = 0;
      *(uint64_t*)(*(uint64_t*)((char*)r14 + 0x188) + 8) = 0;
      sub_fffff8000033453c(*(uint64_t*)((char*)r14 + 0x188), 
        *(uint64_t*)((char*)r14 + 0x158) + 8);
      var_c8 = 1;
    }
    
    rcx_3 = sub_fffff80000336bf0(&arg1[-0x1d8]);
  }
  
  char var_ac = 0;
  
  if (*(uint8_t*)(arg1[-0x157] + 0x6678) && !arg1[-0x1ac] && arg2 != 0x4a)
  {
    (uint8_t)rcx_3 = (arg3 >> 0x1a) & 1;
    var_ac = (uint32_t)rcx_3;
  }
  
  *(uint32_t*)((char*)arg1 - 0xebc) = 0x20;
  
  if (*(int64_t*)((char*)ExceptionList_1 + 0x48))
  {
    __builtin_memset(&var_a8, 0, 0x20);
    (uint32_t)var_a8 = arg2;
    
    if (arg2 != 0x12)
    {
      sub_fffff800003308c4((uint128_t*)((char*)&var_a8 + 8));
      
      if (arg2 != 1)
        rcx_3 = sub_fffff80000236e0c(ExceptionList_1, &arg1[-0x1d8], 1, 
          &var_a8);
    }
    else
    {
      *(uint32_t*)((char*)&var_a8 + 8) =
        sub_fffff80000286a50(&arg1[-0x1d8], nullptr);
      rcx_3 = sub_fffff80000236e0c(ExceptionList_1, &arg1[-0x1d8], 1, 
        &var_a8);
    }
  }
  
  (uint8_t)rcx_3 = *(uint8_t*)(arg1[-0x157] + 0x1a8);
  (uint8_t)rcx_3 &= 0xf;
  int32_t var_c0;
  uint128_t** r12_2;
  uint128_t** rbx_1;
  
  if ((uint8_t)rcx_3 != 3)
  {
    sub_fffff800003309c8(&arg1[-0x1d7]);
    rbx_1 = arg1;
  label_fffff8000035ce66:
    r14[0x92] <<= 1;
    
    if (r14[4])
      r8_1 = sub_fffff8000035dc88(&arg1[-0x1d8], r14, arg2, arg4);
    
    uint64_t var_b8;
    
    if (arg2 == 0x20)
    {
      uint128_t* rax_28 = *(uint64_t*)rbx_1;
      (uint8_t)r8_1 = 1;
      uint32_t r15 = *(uint32_t*)((char*)rax_28 + 8);
      int64_t rbx_4 = rax_28[1] << 0x20 | (uint64_t)*(uint32_t*)rax_28;
      int16_t rax_30;
      rax_30 = sub_fffff8000038a8a8(&arg1[-0x1d8], r15, (uint8_t)r8_1, 
        &var_d8);
      
      if (rax_30)
        goto label_fffff8000035ceda;
      
      uint64_t rcx_14 = &arg1[-0x1d8];
      
      if (var_d8)
      {
        *(uint32_t*)(arg1[-0x1b0] + 0x278) = 0x15;
        r8_1 = sub_fffff8000035a308(rcx_14);
        goto label_fffff8000035ceda;
      }
      
      int16_t rax_32;
      rax_32 = sub_fffff800002cba58(rcx_14, r15, rbx_4, arg4);
      
      if (rax_32)
        goto label_fffff8000035ceda;
      
      int32_t r15_1 = arg1[-0x1d8];
      
      if (((uint64_t)*(uint32_t*)(arg1[-0x157] + 0x1a8) & 0xf) - 2 > 1)
      {
        var_c0 = 0;
        sub_fffff80000225664(&var_c0);
        uint64_t rbx_6 = (uint64_t)arg1[-0x1d7] + var_c0;
        
        if (!sub_fffff800002b75b0(&arg1[-0x1d8]))
          rbx_6 = (uint64_t)(uint32_t)rbx_6;
        
        var_b8 = rbx_6;
        sub_fffff800002297e4(&var_b8);
        arg1[-0x1d8] = 0x1f;
        r8_1 = sub_fffff8000032d29c(&arg1[-0x1d8], 0);
      }
      else
        arg1[-0x1d8] = 0x1f;
      
      r12_2 = arg1;
      
      if (sub_fffff800002e9b1c(r15_1))
        arg1[-0x1d8] = r15_1;
      
      goto label_fffff8000035d52a;
    }
    
    if (arg2 == 1)
    {
      int32_t rax_36;
      rax_36 = sub_fffff8000021bcd0(ExceptionList_1);
      arg1[-0x1d6] = rax_36;
      
      if (rax_36 == 0xffffffff)
      {
        if (*(int64_t*)((char*)ExceptionList_1 + 0x48))
        {
          __builtin_memset((uint128_t*)((char*)&var_a8 + 8), 0, 0x18);
          *(uint32_t*)((char*)&var_a8 + 8) = 0xffffffff;
          r8_1 = sub_fffff80000236e0c(ExceptionList_1, &arg1[-0x1d8], 
            2, &var_a8);
        }
        
        *(uint32_t*)(arg1[-0x1b0] + 0x278) = 0x19;
      }
      else
        r8_1 = sub_fffff800002c3138(ExceptionList_1, &arg1[-0x1d8], 
          r8_1, r9, arg4);
      
      goto label_fffff8000035ceda;
    }
    
    if (arg2 == 0x12)
    {
      r8_1 = sub_fffff80000286ab0(&arg1[-0x1d8]);
    label_fffff8000035ceda:
      r12_2 = arg1;
      goto label_fffff8000035d52a;
    }
    
    if (arg2 == 0xc)
    {
      if (*(uint32_t*)((char*)r13 + 0x15e0) == 3)
      {
        *(uint32_t*)(arg1[-0x1b0] + 0x278) = 0xf;
        r8_1 = sub_fffff8000038b224(&arg1[-0x1d8], 0xc, 0, 
          (uint32_t)arg1[-0x1d7]);
        goto label_fffff8000035ceda;
      }
      
      if (!(*(uint32_t*)(*(uint64_t*)((char*)r14 + 0x28) + 0x128)
          & 0x8000000))
        r8_1 = sub_fffff800002c7e74(&arg1[-0x1d8], arg4);
      else
        r8_1 = sub_fffff800002c84f0(&arg1[-0x1d8], arg4);
      
      goto label_fffff8000035ceda;
    }
    
    if (arg2 == 0x28)
    {
      if (*(uint32_t*)((char*)r13 + 0x15e0) == 3 && *(uint32_t*)(
          *(uint64_t*)(*(uint64_t*)((char*)r13 + 0x15e8) + 8) + 0x314)
          & 0x40000000)
        goto label_fffff8000035d07a;
      
      void* r15_2 = arg1[-0x157];
      
      if (data_fffff800000785e0 && data_fffff800000785c4 & 0x10000000)
      {
        var_c0 = 0;
        sub_fffff80000225664(&var_c0);
        r8_1 = sub_fffff8000025e99c(0x1d3c, var_c0);
      }
      
      if (((uint64_t)*(uint32_t*)(arg1[-0x157] + 0x1a8) & 0xf) - 2 > 1)
      {
        var_c0 = 0;
        sub_fffff80000225664(&var_c0);
        uint64_t rbx_8 = (uint64_t)arg1[-0x1d7] + var_c0;
        
        if (!sub_fffff800002b75b0(&arg1[-0x1d8]))
          rbx_8 = (uint64_t)(uint32_t)rbx_8;
        
        var_b8 = rbx_8;
        sub_fffff800002297e4(&var_b8);
        arg1[-0x1d8] = 0x1f;
        r8_1 = sub_fffff8000032d29c(&arg1[-0x1d8], 0);
      }
      else
        arg1[-0x1d8] = 0x1f;
      
      if (*(uint8_t*)((char*)r15_2 + 0x6217))
      {
        void* rcx_40 = *(uint64_t*)(arg1[-0x1b0] + 0x2538);
        *(uint64_t*)((char*)rcx_40 + 0x200) += 1;
        
        if (*(uint32_t*)((char*)r15_2 + 0x1d0) > 1)
          r8_1 = sub_fffff8000021daa0(gsbase->NtTib.ExceptionList, 2, 
            arg4);
      }
      else
      {
        r8_1 = nullptr;
        void* rcx_39 = &arg1[-0x160][0x13c];
        sub_fffff800003519dc(rcx_39, 
          *(uint32_t*)(*(uint64_t*)((char*)rcx_39 + 0x28) + 0x12c)
            & 0xfffffbff, 
          0);
      }
      
      goto label_fffff8000035ceda;
    }
    
    int16_t rdx_22 = 7;
    
    if (arg2 == 7)
    {
      if (*(uint32_t*)((char*)r13 + 0x15e0) != 3 || !((char)*(uint32_t*)(
        *(uint64_t*)(*(uint64_t*)((char*)r13 + 0x15e8) + 8) + 0x314) & 4))
      {
        sub_fffff800003519a4(r14, 
          *(uint32_t*)(*(uint64_t*)((char*)r14 + 0x28) + 0x128)
            & 0xfffffffb, 
          0);
        r14[0x8d] &= 0xfffffffb;
        r8_1 = sub_fffff800002c9f04(&arg1[-0x1d8]);
        goto label_fffff8000035ceda;
      }
      
      *(uint32_t*)(arg1[-0x1b0] + 0x278) = 0x1b;
    label_fffff8000035d1c8:
      r8_1 = sub_fffff8000038b224(&arg1[-0x1d8], rdx_22, 0, 0);
      goto label_fffff8000035ceda;
    }
    
    rdx_22 = 0x2b;
    
    if (arg2 == 0x2b)
    {
      if (*(uint32_t*)((char*)r13 + 0x15e0) == 3 && *(uint32_t*)(
          *(uint64_t*)(*(uint64_t*)((char*)r13 + 0x15e8) + 8) + 0x314)
          & 0x200000)
        goto label_fffff8000035d1c8;
      
      var_c0 = 0;
      sub_fffff8000033f1b8(&var_c0);
      *(uint8_t*)(*(uint64_t*)((char*)r14 + 0x28) + 0x158) = 0;
      r8_1 = sub_fffff800002c9f04(&arg1[-0x1d8]);
      goto label_fffff8000035ceda;
    }
    
    if (arg2 == 0x1e)
    {
      int64_t var_88;
      sub_fffff800003308c4(&var_88);
      int32_t rbx_9 = (int32_t)var_88;
      (uint8_t)r8_1 = (uint8_t)rbx_9;
      (uint8_t)r8_1 &= 3;
      int128_t var_68;
      int128_t* var_e8_1 = &var_68;
      (uint8_t)r8_1 += 1;
      uint16_t rdx_29 = (int16_t)(rbx_9 >> 0x10);
      *(uint8_t*)((char*)arg1 - 0xeae) = (uint8_t)r8_1;
      *(uint16_t*)((char*)arg1 - 0xeac) = rdx_29;
      __builtin_memset(&var_68, 0, 0x28);
      int16_t rax_50;
      rax_50 = sub_fffff8000038a754(&arg1[-0x1d8], rdx_29, (uint8_t)r8_1, 
        &var_d8, var_e8_1);
      
      if (rax_50 == 0x1007)
      {
        r8_1 = sub_fffff800002d74c4(&arg1[-0x1d8], &var_68);
        goto label_fffff8000035ceda;
      }
      
      if (rax_50)
        goto label_fffff8000035ceda;
      
      if (var_d8)
      {
        *(uint32_t*)(arg1[-0x1b0] + 0x278) = 0xd;
      label_fffff8000035d07a:
        r8_1 = sub_fffff8000035a308(&arg1[-0x1d8]);
        goto label_fffff8000035ceda;
      }
      
      *(uint8_t*)((char*)arg1 - 0xeaf) = ~(char)(rbx_9 >> 3) & 1;
      *(uint8_t*)((char*)arg1 - 0xead) = 0;
      
      if ((uint8_t)rbx_9 & 0x10)
      {
        *(uint8_t*)((char*)arg1 - 0xead) = 1;
        
        if ((uint8_t)rbx_9 & 0x20)
          *(uint8_t*)((char*)arg1 - 0xead) = 3;
      }
      
      *(uint32_t*)((char*)arg1 - 0xebc) = 0xe;
      
      if (!arg1[-0x1ac])
        r8_1 = sub_fffff800002c3d24(&arg1[-0x1d8]);
      else
        r8_1 = sub_fffff800002c4054(&arg1[-0x1d8]);
      
      goto label_fffff8000035ceda;
    }
    
    if (arg2 == 0x38)
    {
      int64_t var_80;
      sub_fffff800003308c4(&var_80);
      uint64_t rcx_53 = (uint64_t)(uint32_t)var_80 & 0xff0;
      r12_2 = arg1;
      uint64_t rbx_10 = (uint64_t)(uint32_t)rcx_53;
      void* r8_5 = *(uint64_t*)(*(uint64_t*)((char*)r14 + 0x28) + 0x118);
      
      if ((uint32_t)rcx_53 != 0x300 || !r12_2[0xd] || !arg1[-0x160][0x3e])
      {
        bool cond:5_1 = !data_fffff800000785e0;
        var_c0 = *(uint32_t*)((char*)r8_5 + rcx_53);
        
        if (!cond:5_1 && data_fffff800000785c4 & 0x2000000)
        {
          var_b8 = 0;
          sub_fffff80000225664(&var_b8);
          sub_fffff8000025ea14(0x1d39, var_b8, rbx_10, 
            (uint64_t)var_c0);
        }
        
        int32_t rdx_31;
        (uint8_t)rdx_31 = 1;
        r8_1 = sub_fffff800002edcfc((uint32_t)rbx_10, (uint8_t)rdx_31, 
          4, &var_c0);
      }
      else
      {
        int64_t r8_6 = *(uint64_t*)((char*)r8_5 + 0x300);
        *(uint32_t*)(arg1[-0x1b0] + 0x278) = 0x15;
        r8_1 = sub_fffff800002ee1f0(&arg1[-0x1d8], 0x830, r8_6, arg4);
      }
      
      goto label_fffff8000035d52a;
    }
    
    if (arg2 == 0x2d)
    {
      int64_t var_78;
      sub_fffff800003308c4(&var_78);
      uint32_t rbx_11 = (uint32_t)(uint8_t)var_78;
      
      if (data_fffff800000785e0 && data_fffff800000785c4 & 0x1000000)
      {
        var_b8 = 0;
        sub_fffff80000225664(&var_b8);
        r8_1 = sub_fffff800002249f4(0x1d38, var_b8, (uint64_t)rbx_11);
      }
      
      if (!TEST_BITQ((uint64_t)arg1[-0x157][0x1a], 0xf))
        r8_1 = sub_fffff800002fc4fc(arg1[-0x160] + 0x80, rbx_11);
      
      goto label_fffff8000035ceda;
    }
    
    if (arg2 != 0x2c)
    {
      if (arg2 == 0x30)
        r8_1 = sub_fffff8000035a7ec(r14, rbx_1, arg4);
      else if (arg2 != 0x31)
        r8_1 = sub_fffff8000035bda0(rbx_1, arg2, arg4);
      else
      {
        sub_fffff800003308f8(&var_b0);
        
        if (var_b0 == 0x10000031)
          r8_1 = sub_fffff800002d89e0(&arg1[-0x1d8]);
      }
      
      goto label_fffff8000035ceda;
    }
    
    int64_t var_70;
    sub_fffff800003308c4(&var_70);
    
    if (((uint32_t)var_70 & 0xf000) < 0x2000)
    {
      arg1[-0x1d8] = 8;
      
      if (data_fffff800000785e0 && data_fffff800000785c4 & 0x4000000)
      {
        var_b8 = 0;
        sub_fffff80000225664(&var_b8);
        sub_fffff8000025e99c(0x1d3a, var_b8);
      }
      
      r8_1 = sub_fffff8000022c4d4(&arg1[-0x1d8], 1, arg4);
      goto label_fffff8000035ceda;
    }
    
    r12_2 = arg1;
    arg1[-0x1d8] = 1;
    r8_1 = sub_fffff800002c2ab4(&arg1[-0x1d8], arg4);
  }
  else
  {
    rbx_1 = arg1;
    arg1[-0x1d7] = 0;
    char rax_27;
    rax_27 = sub_fffff8000035e210(rbx_1, arg2, arg4);
    
    if (!rax_27)
    {
      r13 = arg1[-0x160];
      r14 = (char*)r13 + 0x13c0;
      goto label_fffff8000035ce66;
    }
    
    r12_2 = rbx_1;
  label_fffff8000035d52a:
    
    if (arg1[-0x1d8] != 0x1f)
      r8_1 = sub_fffff800002c2ab4(&arg1[-0x1d8], arg4);
  }
  
  if (!*(uint8_t*)((char*)r12_2 + 0xb1)
    && *(uint32_t*)(*(uint64_t*)((char*)r14 + 0x28) + 0x128) & 0x8000000
    && (arg2 == 9 || arg2 == 0xa || arg2 == 0xb || arg2 == 0xd
    || arg2 == 0xe || arg2 == 0xf || arg2 == 0x10 || arg2 == 0x11
    || arg2 == 0x12 || arg2 == 0x13 || arg2 == 0x14 || arg2 == 0x15
    || arg2 == 0x16 || arg2 == 0x17 || arg2 == 0x18 || arg2 == 0x19
    || arg2 == 0x1a || arg2 == 0x1b || arg2 == 0x1c || arg2 == 0x1d
    || arg2 == 0x1e || arg2 == 0x1f || arg2 == 0x20 || arg2 == 0x24
    || arg2 == 0x27 || arg2 == 0x28 || arg2 == 0x2b || arg2 == 0x2c
    || arg2 == 0x2d || arg2 == 0x2e || arg2 == 0x2f || arg2 == 0x30
    || arg2 == 0x32 || arg2 == 0x33 || arg2 == 0x35 || arg2 == 0x36
    || arg2 == 0x37 || arg2 == 0x38 || arg2 == 0x39 || arg2 == 0x3a
    || arg2 == 0x3b || arg2 == 0x3c || arg2 == 0x3f || arg2 == 0x40))
  {
    var_c0 = 0x80000700;
    sub_fffff80000330b08(&var_c0);
  }
  
  if (var_ac)
    r8_1 = sub_fffff8000035a3c4(&arg1[-0x1d8], arg4);
  
  sub_fffff8000035aac0(r12_2, arg4, r8_1, arg5);
  uint64_t result = arg1[-0x160];
  
  if (data_fffff80000122710 <= 1 && *(uint32_t*)(result + 0x15e0) == 3)
    result = sub_fffff8000035d784();
  
  if (var_c8)
  {
    uint64_t* rcx_75 = *(uint64_t*)((char*)r14 + 0x188);
    
    if (*(uint32_t*)((char*)rcx_75 + 0x6c) == 3)
    {
      sub_fffff800003358e0(rcx_75, *(uint64_t*)((char*)r14 + 0x158) + 8);
      void* rax_59 = *(uint64_t*)((char*)r14 + 0x158);
      *(uint64_t*)((char*)rax_59 + 8) &= 0xfffffffffffffffe;
      result = *(uint64_t*)((char*)r14 + 0x188);
      *(uint32_t*)(result + 0x6c) = 2;
    }
  }
  
  struct _EXCEPTION_REGISTRATION_RECORD* ExceptionList =
    gsbase->NtTib.ExceptionList;
  
  if (*(int64_t*)((char*)ExceptionList + 0x48))
    result = sub_fffff80000236e0c(ExceptionList, &arg1[-0x1d8], 3, &var_a8);
  
  sub_fffff800003a3fa0(rax_1 ^ &var_108);
  return result;
}

void* sub_fffff800003308c4(int64_t* arg1)
{
  int64_t arg_10 = 0;
  
  if (!(data_fffff800000af0b0 & 1))
  {
    uint64_t rflags;
    int64_t temp0;
    temp0 = __vmread_memq_gpr64(0x6400);
    *(uint64_t*)arg1 = temp0;
    return 0x6400;
  }
  
  TEB* gsbase;
  void* result = *(int64_t*)((char*)gsbase + 0x2c680);
  *(uint64_t*)arg1 = *(uint64_t*)((char*)result + 0x2d0);
  return result;
}
