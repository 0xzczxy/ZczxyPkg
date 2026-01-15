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

uint128_t* sub_fffff8000035bda0(uint128_t** arg1, int32_t arg2, int512_t arg3 @ zmm1)
{
  void var_108;
  int64_t rax_1 = __security_cookie ^ &var_108;
  uint64_t rsi = (uint64_t)arg2;
  int32_t var_c0 = 0;
  void* rbx = arg1[-0x160];
  int64_t var_b0;
  __builtin_memset(&var_b0, 0, 0x18);
  uint64_t var_d8 = 0;
  int32_t var_b8 = 0;
  uint32_t var_c8 = 0;
  int32_t* r9 = (char*)rbx + 0x13c0;
  char var_d0 = 0;
  uint128_t* result;
  
  if (!arg2)
  {
    sub_fffff80000330a64(&var_c0);
    int32_t rdx_17 = var_c0;
    result = (uint64_t)rdx_17 & 0x700;
    
    if ((uint32_t)result != 0x200)
      result = sub_fffff8000035dd14(arg1, rdx_17, arg3);
    else
      sub_fffff800003a3fd0();
  }
  else
  {
    int16_t rdx_8;
    
    if (arg2 != 2)
    {
      if (arg2 == 3)
      {
        sub_fffff800002c9c70();
        /* no return */
      }
      
      if (arg2 == 8)
      {
        if (*(uint32_t*)((char*)rbx + 0x15e0) != 3 || !(*(uint32_t*)(
          *(uint64_t*)(*(uint64_t*)((char*)rbx + 0x15e8) + 8) + 0x314)
          & 0x400000))
        {
          sub_fffff800003519a4(r9, 
            *(uint32_t*)(*(uint64_t*)((char*)r9 + 0x28) + 0x128)
              & 0xffbfffff, 
            0);
          r9[0x8d] &= 0xffbfffff;
          result = sub_fffff800002c9f04(&arg1[-0x1d8]);
        }
        else
        {
          *(uint32_t*)(arg1[-0x1b0] + 0x278) = 0x1b;
          sub_fffff8000032da10(&var_c8);
          uint32_t rcx_96 = var_c8;
          
          if (!((uint8_t)rcx_96 & 1))
          {
            rdx_8 = 8;
            goto label_fffff8000035ca43;
          }
          
          var_c8 = (rcx_96 & 0xfffffffe) | 2;
          result = sub_fffff8000032db40(&var_c8);
        }
      }
      else if (arg2 == 9)
        result = sub_fffff8000035a934(r9, &arg1[-0x1d8], arg3);
      else
      {
        int64_t r8_1 = 3;
        int32_t var_e8;
        int128_t* var_e0_1;
        void var_98;
        int32_t* rcx_73;
        char* rdx;
        TEB* gsbase;
        
        switch (arg2)
        {
          case 0xa:
          {
            rcx_73 = &arg1[-0x1d8];
            
            if (*(uint32_t*)((char*)rbx + 0x15e0) != 3)
              result = sub_fffff800002c7b98(rcx_73);
            else
            {
              *(uint32_t*)(arg1[-0x1b0] + 0x278) = 0x13;
              result = sub_fffff8000035a308(rcx_73);
            }
            break;
          }
          case 0xd:
          {
            if (*(uint32_t*)((char*)rbx + 0x15e0) != 3)
              result = sub_fffff800002cabf8(&arg1[-0x1d8]);
            else
              result = sub_fffff8000035a308(&arg1[-0x1d8]);
            break;
          }
          case 0xe:
          {
            sub_fffff800003308c4(&arg1[-0x1d6]);
            
            if (*(uint32_t*)((char*)rbx + 0x15e0) != 3 || !(*(
              uint32_t*)(
              *(uint64_t*)(*(uint64_t*)((char*)rbx + 0x15e8) + 8)
              + 0x314) & 0x200))
            {
            label_fffff8000035c964:
              
              if (arg1[-0x1ac])
              {
                sub_fffff8000021f21c(6, 0, rsi, 0, 0, 
                  __return_addr);
                /* no return */
              }
              
              result = sub_fffff800002c5d7c(&arg1[-0x1d8], arg3);
            }
            else
            {
              *(uint32_t*)(arg1[-0x1b0] + 0x278) = 9;
              result = sub_fffff8000038b224(&arg1[-0x1d8], 0xe, 
                arg1[-0x1d6], (uint32_t)arg1[-0x1d7]);
            }
            break;
          }
          case 0xf:
          {
            if (*(uint32_t*)((char*)rbx + 0x15e0) != 3 || !(*(
                uint32_t*)(*(uint64_t*)(
                *(uint64_t*)((char*)rbx + 0x15e8) + 8) + 0x314)
                & 0x800))
              result = sub_fffff800002c8128(&arg1[-0x1d8], arg3);
            else
              result = sub_fffff8000035a308(&arg1[-0x1d8]);
            break;
          }
🔖          case 0x10:
          {
            if (*(uint32_t*)((char*)rbx + 0x15e0) != 3 || !(*(
                uint32_t*)(*(uint64_t*)(
                *(uint64_t*)((char*)rbx + 0x15e8) + 8) + 0x314)
                & 0x1000))
              result =
                sub_fffff800002c8278(&arg1[-0x1d8], 0, arg3);
            else
              result = sub_fffff8000035a308(&arg1[-0x1d8]);
            break;
          }
          case 0x13:
          case 0x14:
          case 0x15:
          case 0x16:
          case 0x17:
          case 0x18:
          case 0x19:
          case 0x1a:
          case 0x1b:
          {
          label_fffff8000035c88b:
            sub_fffff8000035d750(&var_b8);
            sub_fffff800003308c4(&var_d8);
            result = sub_fffff8000038a144(&arg1[-0x1d8], 
              (uint32_t)rsi, var_b8, var_d8);
            break;
          }
          case 0x1c:
          {
            result = sub_fffff8000035a5e4(&arg1[-0x1d8]);
            break;
          }
          case 0x1d:
          {
            if (*(uint32_t*)((char*)rbx + 0x15e0) != 3 || !(*(
                uint32_t*)(*(uint64_t*)(
                *(uint64_t*)((char*)rbx + 0x15e8) + 8) + 0x314)
                & 0x800000))
              result = sub_fffff800002c7dc8(&arg1[-0x1d8]);
            else
            {
              *(uint32_t*)(arg1[-0x1b0] + 0x278) = 0x1f;
              sub_fffff800003308c4(&var_d8);
              var_c8 = 0;
              sub_fffff800003309c8(&var_c8);
              result = sub_fffff8000038b224(&arg1[-0x1d8], 0x1d, 
                var_d8, var_c8);
            }
            break;
          }
          case 0x1f:
          {
            int32_t rbx_8 = *(uint32_t*)(*(uint64_t*)arg1 + 8);
            int512_t zmm1_1;
            result = sub_fffff8000038a8a8(&arg1[-0x1d8], rbx_8, 0, 
              &var_d0);
            
            if (!(uint16_t)result)
            {
              rcx_73 = &arg1[-0x1d8];
              
              if (!var_d0)
              {
                result = sub_fffff800002cb5a8(rcx_73, rbx_8, 
                  &var_b0, zmm1_1);
                
                if (!(uint16_t)result)
                {
                  (*(uint64_t*)arg1)[1] = var_b0 >> 0x20;
                  **(uint64_t**)arg1 =
                    (uint64_t)(uint32_t)var_b0;
                label_fffff8000035c133:
                  result = arg1[-0x157];
                  
                  if (((uint64_t)
                    *(uint32_t*)((char*)result + 0x1a8) & 0xf)
                    - 2 > 1)
                  {
                  label_fffff8000035c291:
                    var_d8 = 0;
                    sub_fffff80000225664(&var_d8);
                    uint64_t rbx_2 =
                      (uint64_t)arg1[-0x1d7] + var_d8;
                    
                    if (
                        !sub_fffff800002b75b0(&arg1[-0x1d8]))
                      rbx_2 = (uint64_t)(uint32_t)rbx_2;
                    
                    var_d8 = rbx_2;
                    sub_fffff800002297e4(&var_d8);
                    arg1[-0x1d8] = 0x1f;
                    result =
                      sub_fffff8000032d29c(&arg1[-0x1d8], 0);
                  }
                  else
                    arg1[-0x1d8] = 0x1f;
                }
              }
              else
              {
                *(uint32_t*)(arg1[-0x1b0] + 0x278) = 0x15;
                result = sub_fffff8000035a308(rcx_73);
              }
            }
            break;
          }
          case 0x21:
          {
            r9[0x92] u>>= 1;
            sub_fffff8000033085c(&var_c0);
            int32_t rcx_64 = var_c0;
            
            if (rcx_64 < 0 && (rcx_64 & 0x700) != 0x700)
            {
              *(uint32_t*)r9 = rcx_64;
              var_c0 = rcx_64 & 0x7fffffff;
              sub_fffff80000330b08(&var_c0);
            }
            
            int512_t zmm1 = sub_fffff8000021f1b0(r9);
            *(uint8_t*)(*(uint64_t*)((char*)rbx + 0x13e8)
              + 0x126) = 0;
            void* rcx_68 = *(uint64_t*)((char*)rbx + 0x13e8);
            void* rdx_9 = *(uint64_t*)((char*)rcx_68 + 0x180);
            
            if (!rdx_9)
            {
              var_d8 = *(int64_t*)(
                (char*)gsbase->NtTib.ExceptionList + 0x29ec0);
              __vmptrld_memq(var_d8);
              __vmclear_memq(var_d8);
            }
            else if (!
                ((char)(*(uint32_t*)data_fffff800000af0b0) & 1))
              __vmptrld_memq(*(uint64_t*)((char*)rcx_68 + 0x188));
            else
            {
              int64_t rcx_69 =
                *(uint64_t*)((char*)rcx_68 + 0x188);
              *(uint32_t*)((char*)rdx_9 + 0x340) = 1;
              struct _EXCEPTION_REGISTRATION_RECORD* 
                ExceptionList_1 = gsbase->NtTib.ExceptionList;
              *(int64_t*)((char*)ExceptionList_1 + 0x2c680) =
                rdx_9;
              *(uint64_t*)(
                *(int64_t*)((char*)ExceptionList_1 + 0x2c4c8)
                + 0x30) = rcx_69;
            }
            
            result = sub_fffff8000035a360(&arg1[-0x1d8], zmm1);
            break;
          }
          case 0x22:
          {
            sub_fffff800003308c4(&var_d8);
            var_c8 = 0;
            uint64_t rbx_5;
            
            if (!(data_fffff800000af0b0 & 1))
            {
              uint64_t rflags_5;
              rbx_5 = __vmread_gpr64_gpr64(0x200a);
              var_c8 = rbx_5;
            }
            else
              rbx_5 = *(uint64_t*)(
                *(int64_t*)((char*)gsbase + 0x2c680) + 0x150);
            
            int64_t r10_4 = *(uint64_t*)((char*)r9 + 0x158);
            int64_t rax_72;
            rax_72 = sub_fffff800002633e4(r10_4);
            uint64_t rax_73 = var_d8;
            int128_t var_a8_1;
            
            if (rax_72 != rbx_5)
            {
              int64_t rbx_6 = rbx_5 + ((rax_73 - 1) << 4);
              uint8_t* rax_77 =
                sub_fffff800002623b8(rbx_6 >> 0xc);
              var_a8_1 = *(uint128_t*)(((uint64_t)(uint32_t)rbx_6
                & 0xfff) + rax_77);
              r8_1 = sub_fffff80000262408(rax_77);
            }
            else
              var_a8_1 =
                *(uint128_t*)(r10_4 + rax_73 * 0x10 - 0x10);
            
            if (arg1[-0x1ac])
            {
              sub_fffff8000021f21c(6, 0xb, 0x22, 
                (uint64_t)(uint32_t)var_a8_1, 
                *(uint64_t*)((char*)&var_a8_1 + 8), 
                __return_addr);
              /* no return */
            }
            
            if (data_fffff800000232e8 <= 5)
              result = sub_fffff800002cacc0(&arg1[-0x1d8], arg3);
            else
            {
              rdx = &data_fffff80000013af9;
              int64_t var_50_2 = 4;
              int64_t var_40_1 = 8;
              uint64_t rcx_61 = arg1[-0x157][0x455];
              uint64_t* var_78_2 = &var_d8;
              var_c8 = *(uint32_t*)((char*)arg1 - 0xd5c);
              uint32_t* var_68_2 = &var_c8;
              var_c0 = (uint32_t)var_a8_1;
              int32_t* var_58_2 = &var_c0;
              var_b8 = *(uint64_t*)((char*)&var_a8_1 + 8);
              int32_t* var_48_1 = &var_b8;
              var_e0_1 = &var_98;
              var_e8 = 6;
              var_d8 = rcx_61;
            label_fffff8000035c6b3:
              int64_t var_70_1 = 8;
              int64_t var_60_1 = 4;
              result = sub_fffff800002cacc0(&arg1[-0x1d8], 
                sub_fffff80000200008(&data_fffff800000232e8, 
                  rdx, r8_1, r9, var_e8, var_e0_1));
            }
            break;
          }
          case 0x24:
          {
          label_fffff8000035c56c:
            int32_t rcx_55 = *(uint32_t*)((char*)rbx + 0x15e0);
            result = arg1[-0x1b0];
            *(uint32_t*)((char*)result + 0x278) = 0x11;
            
            if (rcx_55 != 3)
            {
            label_fffff8000035bfab:
              *(uint64_t*)((char*)arg1 - 0xeac) = 6;
              arg1[-0x1d6] = 0;
              arg1[-0x1d4] = 0;
              arg1[-0x1d8] = 7;
            }
            else
              result = sub_fffff8000038b224(&arg1[-0x1d8], 
                (uint16_t)rsi, 0, (uint32_t)arg1[-0x1d7]);
            break;
          }
          case 0x25:
          {
            result = sub_fffff800002c84f0(&arg1[-0x1d8], arg3);
            break;
          }
          default:
          {
            if (arg2 == 0x27)
              goto label_fffff8000035c56c;
            
            if (arg2 == 0x29)
            {
              r9[0x92] u>>= 1;
              sub_fffff8000033085c(&var_c0);
              int32_t rcx_47 = var_c0;
              
              if (rcx_47 < 0 && (rcx_47 & 0x700) != 0x700)
              {
                *(uint32_t*)r9 = rcx_47;
                var_c0 = rcx_47 & 0x7fffffff;
                sub_fffff80000330b08(&var_c0);
              }
              
              sub_fffff8000021f1b0(r9);
              *(uint8_t*)(*(uint64_t*)((char*)rbx + 0x13e8)
                + 0x126) = 0;
              void* rcx_51 = *(uint64_t*)((char*)rbx + 0x13e8);
              void* rdx_7 = *(uint64_t*)((char*)rcx_51 + 0x180);
              
              if (!rdx_7)
              {
                var_d8 = *(int64_t*)(
                  (char*)gsbase->NtTib.ExceptionList
                  + 0x29ec0);
                __vmptrld_memq(var_d8);
                __vmclear_memq(var_d8);
              }
              else if (!((char)(*(uint32_t*)data_fffff800000af0b0)
                  & 1))
                __vmptrld_memq(
                  *(uint64_t*)((char*)rcx_51 + 0x188));
              else
              {
                int64_t rcx_52 =
                  *(uint64_t*)((char*)rcx_51 + 0x188);
                *(uint32_t*)((char*)rdx_7 + 0x340) = 1;
                struct _EXCEPTION_REGISTRATION_RECORD* 
                  ExceptionList = gsbase->NtTib.ExceptionList;
                *(int64_t*)((char*)ExceptionList + 0x2c680) =
                  rdx_7;
                *(uint64_t*)(
                  *(int64_t*)((char*)ExceptionList + 0x2c4c8)
                  + 0x30) = rcx_52;
              }
              
              result = sub_fffff800002c9c98(&arg1[-0x1d8]);
            }
            else if (arg2 == 0x2e || arg2 == 0x2f)
            {
              char rax_62;
              
              if (*(uint32_t*)((char*)rbx + 0x15e0) == 3)
                rax_62 = sub_fffff8000034e47c(
                  *(uint64_t*)(
                    *(uint64_t*)((char*)rbx + 0x15e8) + 8), 
                  4);
              
              if (*(uint32_t*)((char*)rbx + 0x15e0) == 3
                && rax_62)
              {
                *(uint32_t*)(arg1[-0x1b0] + 0x278) = 0x1d;
                sub_fffff800003308c4(&var_d8);
                sub_fffff8000035d750(&var_b8);
                int32_t rbx_4 = var_b8 & 0x3fffefff;
                char rax_64;
                
                if ((uint32_t)rsi != 0x2f)
                  rax_64 =
                    sub_fffff800002b75b0(&arg1[-0x1d8]);
                
                if ((uint32_t)rsi == 0x2f || rax_64)
                  rbx_4 &= 0xffffe7ff;
                
                var_c8 = 0;
                sub_fffff800003309c8(&var_c8);
                var_e8 = rbx_4;
                goto label_fffff8000035c0c3;
              }
              
              sub_fffff8000035d750(&var_b8);
              sub_fffff800003308c4(&var_d8);
              uint64_t r8_8 = var_d8;
              int32_t rdx_6 = var_b8;
              
              if ((uint32_t)rsi != 0x2e)
                result = sub_fffff80000389b64(&arg1[-0x1d8], 
                  rdx_6, r8_8, arg3);
              else
                result = sub_fffff800003898cc(&arg1[-0x1d8], 
                  rdx_6, r8_8);
            }
            else
            {
              if (arg2 == 0x32)
                goto label_fffff8000035c88b;
              
              if (arg2 != 0x33)
              {
                if (arg2 == 0x35)
                  goto label_fffff8000035c88b;
                
                if (arg2 == 0x36)
                  result =
                    sub_fffff800002cabf8(&arg1[-0x1d8]);
                else if (arg2 == 0x37)
                {
                  if (*(uint32_t*)((char*)rbx + 0x15e0) == 3)
                    result =
                      sub_fffff8000035a308(&arg1[-0x1d8]);
                  else
                    result = sub_fffff800002c85a0(
                      &arg1[-0x1d8], arg3);
                }
                else if (arg2 == 0x39)
                {
                  if (*(uint32_t*)((char*)rbx + 0x15e0) != 3)
                    goto label_fffff8000035c964;
                  
                  if (!sub_fffff8000034e47c(
                      *(uint64_t*)(
                        *(uint64_t*)((char*)rbx + 0x15e8) + 8), 
                      0x800))
                    goto label_fffff8000035c964;
                  
                  sub_fffff800003308c4(&var_d8);
                  sub_fffff8000035d750(&var_b8);
                  int32_t r8_6 = var_b8 & 0xffffbff;
                  var_c8 = 0;
                  sub_fffff800003309c8(&var_c8);
                  result = sub_fffff8000038b16c(
                    &arg1[-0x1d8], 0x39, var_d8, var_c8, 
                    r8_6 | 0x400);
                }
                else if (arg2 == 0x3a)
                {
                  if (*(uint32_t*)((char*)rbx + 0x15e0) != 3)
                    goto label_fffff8000035c964;
                  
                  sub_fffff800003308c4(&var_d8);
                  sub_fffff8000035d750(&var_b8);
                  int32_t r8_3 = var_b8;
                  int32_t rax_59 = r8_3 >> 7 & 7;
                  uint64_t r10_3;
                  
                  if (rax_59)
                  {
                    r10_3 = (uint64_t)(uint32_t)var_d8;
                    
                    if (rax_59 != 1)
                      r10_3 = var_d8;
                  }
                  else
                    r10_3 = (uint64_t)(uint16_t)var_d8;
                  
                  var_c8 = 0;
                  sub_fffff800003309c8(&var_c8);
                  result = sub_fffff8000038b16c(
                    &arg1[-0x1d8], 0x3a, r10_3, var_c8, 
                    r8_3);
                }
                else if (arg2 == 0x3b)
                {
                  result = *(uint64_t*)arg1;
                  
                  if (*(uint32_t*)result)
                    goto label_fffff8000035bfab;
                  
                  char rcx_17 = 0;
                  
                  if (!(data_fffff800000af0b0 & 1))
                  {
                    result = 0x2018;
                    uint64_t rflags_1;
                    rcx_17 = __vmread_gpr64_gpr64(0x2018);
                  }
                  
                  if (!(rcx_17 & 1))
                    goto label_fffff8000035bfab;
                  
                  result = *(uint64_t*)arg1;
                  int32_t rcx_18 = (int32_t)
                    *(uint64_t*)((char*)result + 8);
                  
                  if (rcx_18 >= 0x200)
                    goto label_fffff8000035bfab;
                  
                  result = *(uint64_t*)((char*)r9 + 0x150);
                  uint64_t rdx_4 = *(uint64_t*)((char*)result
                    + ((uint64_t)rcx_18 << 3));
                  
                  if (rdx_4 == -1)
                    goto label_fffff8000035bfab;
                  
                  var_d8 = rdx_4;
                  sub_fffff80000353020(&var_d8);
                  result = ((uint64_t)
                    *(uint32_t*)(arg1[-0x157] + 0x1a8) & 0xf)
                    - 2;
                  
                  if (result > 1)
                    goto label_fffff8000035c291;
                  
                  arg1[-0x1d8] = 0x1f;
                }
                else if (arg2 == 0x3c)
                {
                  result = arg1[-0x157];
                  
                  if (!(result[0x1a] & 0x10))
                    goto label_fffff8000035bfab;
                  
                  result = *(uint64_t*)arg1;
                  int32_t r9_3 = (int32_t)*(uint64_t*)result;
                  
                  if (*(uint32_t*)((char*)rbx + 0x15e0) != 3)
                  {
                  label_fffff8000035c1eb:
                    
                    if (r9_3 != 2)
                    {
                      *(uint64_t*)((char*)arg1 - 0xeac) = 0xd;
                      arg1[-0x1d6] = 1;
                      arg1[-0x1d4] = 0;
                      arg1[-0x1d8] = 7;
                    }
                    else
                      result =
                        sub_fffff80000330704(&arg1[-0x1d8]);
                  }
                  else
                  {
                    void* r8_2 = *(uint64_t*)(
                      *(uint64_t*)((char*)rbx + 0x15e8) + 8);
                    result =
                      sub_fffff8000034e47c(r8_2, 0x8000);
                    
                    if (!(uint8_t)result)
                      goto label_fffff8000035c1eb;
                    
                    int32_t rax_53 = r9_3;
                    
                    if (r9_3 >= 0x3f)
                      rax_53 = 0x3f;
                    
                    result =
                      *(uint64_t*)((char*)r8_2 + 0x3c8);
                    
                    if (
                        !TEST_BITQ(result, (uint64_t)rax_53))
                      goto label_fffff8000035c1eb;
                    
                    result =
                      sub_fffff8000035a308(&arg1[-0x1d8]);
                  }
                }
                else if (arg2 != 0x3f && arg2 != 0x40)
                {
                  result = (uint64_t)(arg2 - 0x41);
                  
                  if (arg2 == 0x41)
                    goto label_fffff8000035bfab;
                  
                  if ((uint32_t)result == 2
                    || (uint32_t)result == 3)
                  {
                    if (
                        *(uint32_t*)((char*)rbx + 0x15e0) == 3
                        && *(uint32_t*)(*(uint64_t*)(
                        *(uint64_t*)((char*)rbx + 0x15e8) + 8)
                        + 0x314) & 0x1000)
                      goto label_fffff8000035c0a1;
                    
                    var_d8 = 0;
                    sub_fffff8000022984c(r9, &var_d8);
                    void* r11_1 = arg1[-0x160];
                    int64_t r10_2 =
                      var_d8 & 0xfffffffffffff72a;
                    var_d8 = 0;
                    sub_fffff8000022984c(
                      (char*)r11_1 + 0x13c0, &var_d8);
                    sub_fffff800003520bc(
                      (char*)r11_1 + 0x13c0, r10_2);
                    
                    if (TEST_BITQ(var_d8 ^ r10_2, 8))
                    {
                      void* rax_51 =
                        *(uint64_t*)((char*)r11_1 + 0x13e8);
                      *(uint8_t*)((char*)r11_1 + 0x13f1) = 1;
                      *(uint8_t*)((char*)rax_51 + 0x125) = 1;
                    }
                    
                    goto label_fffff8000035c133;
                  }
                  
                  if ((uint32_t)result == 9)
                  {
                    result = arg1[-0x1b0];
                    *(uint32_t*)((char*)result + 0x278) =
                      0xc8;
                    
                    if (!arg1[-0x1ac])
                    {
                      result = arg1[-0x157];
                      
                      if (*(uint8_t*)((char*)result + 0x6678))
                        result = sub_fffff8000035a3c4(
                          &arg1[-0x1d8], arg3);
                    }
                  }
                  else
                  {
                    int32_t rax_46 = (uint32_t)result - 0xa;
                    
                    if ((uint32_t)result != 0xa)
                    {
                      result = (uint64_t)(rax_46 - 1);
                      
                      if (rax_46 == 1
                          || (uint32_t)result == 1)
                        goto label_fffff8000035bfab;
                      
                      goto label_fffff8000035c169;
                    }
                    
                    if (arg1[-0x1ac])
                    {
                      sub_fffff8000021f21c(0x33, 0, 
                        &arg1[-0x1d8], 0, 0, __return_addr);
                      /* no return */
                    }
                    
                    if (data_fffff800000232e8 > 5)
                    {
                      rdx = &data_fffff80000013b47;
                      int64_t var_50_1 = 1;
                      uint64_t rcx = arg1[-0x157][0x455];
                      int64_t* var_78_1 = &var_d8;
                      var_c8 =
                        *(uint32_t*)((char*)arg1 - 0xd5c);
                      uint32_t* var_68_1 = &var_c8;
                      var_d8 = rcx;
                      var_d0 = *(uint8_t*)((char*)rbx + 0x14);
                      char* var_58_1 = &var_d0;
                      var_e0_1 = &var_98;
                      var_e8 = 5;
                      goto label_fffff8000035c6b3;
                    }
                    
                    result = sub_fffff800002cacc0(
                      &arg1[-0x1d8], arg3);
                  }
                }
                else if (*(uint32_t*)((char*)rbx + 0x15e0) == 3)
                {
                label_fffff8000035c0a1:
                  sub_fffff800003308c4(&var_d8);
                  sub_fffff8000035d750(&var_b8);
                  var_c8 = 0;
                  sub_fffff800003309c8(&var_c8);
                  var_e8 = var_b8;
                label_fffff8000035c0c3:
                  result = sub_fffff8000038b16c(
                    &arg1[-0x1d8], (uint32_t)rsi, var_d8, 
                    var_c8, var_e8);
                }
                else
                {
                label_fffff8000035c169:
                  
                  if ((int32_t)rsi >= 0)
                    goto label_fffff8000035c964;
                  
                  if (*(uint32_t*)((char*)rbx + 0x15e0) != 3)
                  {
                    sub_fffff8000035aa4c((uint16_t)rsi);
                    /* no return */
                  }
                  
                  result = sub_fffff800002c5cd4(
                    &arg1[-0x1d8], arg3);
                }
              }
              else if (*(uint32_t*)((char*)rbx + 0x15e0) != 3 || !
                (*(uint32_t*)(*(uint64_t*)(
                *(uint64_t*)((char*)rbx + 0x15e8) + 8) + 0x314)
                & 0x1000))
              {
                (uint8_t)arg2 = 1;
                result = sub_fffff800002c8278(&arg1[-0x1d8], 
                  (uint8_t)arg2, arg3);
              }
              else
                result = sub_fffff8000035a308(&arg1[-0x1d8]);
            }
          }
        }
      }
    }
    else if (*(uint32_t*)((char*)rbx + 0x15e0) != 3)
      result = sub_fffff800002c84f0(&arg1[-0x1d8], arg3);
    else
    {
      rdx_8 = 2;
    label_fffff8000035ca43:
      result = sub_fffff8000038b224(&arg1[-0x1d8], rdx_8, 0, 0);
    }
  }
  
  sub_fffff800003a3fa0(rax_1 ^ &var_108);
  return result;
}

void* sub_fffff800002c8278(int32_t* arg1, char arg2, int512_t arg3 @ zmm1)
{
  void var_c8;
  int64_t rax_1 = __security_cookie ^ &var_c8;
  void* rax_2 = *(uint64_t*)((char*)arg1 + 0x140);
  int64_t rsi;
  (uint8_t)rsi = arg2;
  uint64_t var_98;
  __builtin_memset(&var_98, 0, 0x20);
  *(uint32_t*)((char*)rax_2 + 0x278) = 0x1d;
  void* result;
  
  if (arg2)
    result = *(uint64_t*)((char*)arg1 + 0x408);
  
  if (!arg2 || TEST_BITQ(*(uint64_t*)((char*)result + 0x6080), 0x26))
  {
    void* var_90;
    sub_fffff800002b7100(arg1, 0x41003, &var_90);
    int64_t var_88;
    sub_fffff800002b7100(arg1, 0x41000, &var_88);
    result = sub_fffff80000351a94(arg1);
    
    if ((uint8_t)var_90 & 4 && (uint8_t)result && (uint8_t)var_88 & 1)
    {
      arg1[6] = 0;
      *(uint64_t*)((char*)arg1 + 0x20) = 0;
      arg1[5] = 0xd;
      arg1[4] = 1;
      *(uint32_t*)arg1 = 7;
    }
    else if (!(*(uint8_t*)(*(uint64_t*)((char*)arg1 + 0x3c0) + 0x1340) & 4))
    {
      int64_t var_80;
      int512_t zmm1_2 = sub_fffff800002b5c00(0x80000, &var_80, arg3);
      int64_t rdx_6 = var_80;
      **(uint64_t**)((char*)arg1 + 0xec0) = (uint64_t)(uint32_t)rdx_6;
      *(uint64_t*)(*(uint64_t*)((char*)arg1 + 0xec0) + 0x10) =
        rdx_6 >> 0x20;
      
      if ((uint8_t)rsi)
      {
        sub_fffff800002b5c00(0x8007b, &var_98, zmm1_2);
        *(uint64_t*)(*(uint64_t*)((char*)arg1 + 0xec0) + 8) = var_98;
      }
      
      result = *(uint64_t*)((char*)arg1 + 0x408);
      
      if (((uint64_t)*(uint32_t*)((char*)result + 0x1a8) & 0xf) - 2 > 1)
      {
        var_98 = 0;
        sub_fffff80000225664(&var_98);
        uint64_t rdi_2 = (uint64_t)arg1[2] + var_98;
        
        if (!sub_fffff800002b75b0(arg1))
          rdi_2 = (uint64_t)(uint32_t)rdi_2;
        
        var_98 = rdi_2;
        sub_fffff800002297e4(&var_98);
        *(uint32_t*)arg1 = 0x1f;
        result = sub_fffff8000032d29c(arg1, 0);
      }
      else
        *(uint32_t*)arg1 = 0x1f;
    }
    else
    {
      void var_78;
      sub_fffff800003b2640(&var_78, 0, 0x60);
      sub_fffff800002ca8e0(arg1, 2, 1, &var_78);
      int512_t zmm1;
      void* var_40;
      void* var_38;
      
      if (!((*(uint64_t*)data_fffff800000af0b8) & 0x8000000))
      {
        zmm1 = sub_fffff800002b5c00(0x10000005, &var_40, arg3);
        var_38 =
          sub_fffff80000274c6c(*(uint64_t*)((char*)arg1 + 0x408));
      }
      else
      {
        void* rax_4;
        rax_4 = sub_fffff800002b5a88(arg1, &var_38, arg3);
        var_40 = rax_4;
      }
      void var_48;
      int512_t zmm1_1 = sub_fffff800002b5c00(0x80087, &var_48, zmm1);
      
      if ((uint8_t)rsi)
      {
        void var_50;
        zmm1_1 = sub_fffff800002b5c00(0x8007b, &var_50, zmm1_1);
        int64_t var_20;
        int64_t var_20_1 = var_20 | 1;
      }
      
      result = sub_fffff800002c6300(arg1, 0, 0x8001000a, 0x60, zmm1_1, 
        &var_78, 0);
    }
  }
  else
  {
    arg1[6] = 0;
    *(uint64_t*)((char*)arg1 + 0x20) = 0;
    arg1[5] = 6;
    arg1[4] = 0;
    *(uint32_t*)arg1 = 7;
  }
  
  sub_fffff800003a3fa0(rax_1 ^ &var_c8);
  return result;
}

int64_t sub_fffff8000035a308(int32_t* arg1)
{
  int32_t arg_10 = 0;
  int64_t arg_20 = 0;
  int32_t arg_18 = 0;
  sub_fffff800003308f8(&arg_18);
  sub_fffff800003308c4(&arg_20);
  sub_fffff800003309c8(&arg_10);
  return sub_fffff8000038b224(arg1, (int16_t)arg_18, arg_20, arg_10);
}

int64_t sub_fffff800003308f8(int32_t* arg1)
{
  int64_t arg_10 = 0;
  
  if (!(data_fffff800000af0b0 & 1))
  {
    uint64_t rflags;
    int32_t rax_1;
    rax_1 = __vmread_gpr64_gpr64(0x4402);
    *(uint32_t*)arg1 = rax_1;
    return rax_1;
  }
  
  TEB* gsbase;
  void* rax = *(int64_t*)((char*)gsbase + 0x2c680);
  *(uint32_t*)arg1 = *(uint32_t*)((char*)rax + 0x2b4);
  return rax;
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

int64_t sub_fffff800003309c8(int32_t* arg1)
{
  int64_t arg_10 = 0;
  
  if (!(data_fffff800000af0b0 & 1))
  {
    uint64_t rflags;
    int32_t rax_1;
    rax_1 = __vmread_gpr64_gpr64(0x440c);
    *(uint32_t*)arg1 = rax_1;
    return rax_1;
  }
  
  TEB* gsbase;
  void* rax = *(int64_t*)((char*)gsbase + 0x2c680);
  *(uint32_t*)arg1 = *(uint32_t*)((char*)rax + 0x2c8);
  return rax;
}

int64_t sub_fffff8000038b224(int32_t* arg1, int16_t arg2, int64_t arg3, int32_t arg4)
{
  int64_t* r11 = *(uint64_t*)((char*)arg1 + 0x3c0);
  void* r10 = *(uint64_t*)(r11[0x2bd] + 8);
  *(uint32_t*)((char*)r10 + 0x2b4) = 0;
  *(uint32_t*)((char*)r10 + 0x2b8) &= 0x7fffffff;
  *(uint32_t*)((char*)r10 + 0x2c0) &= 0x7fffffff;
  *(uint32_t*)((char*)r10 + 0x2bc) = 0;
  *(uint16_t*)((char*)r10 + 0x2b4) = arg2;
  *(uint64_t*)((char*)r10 + 0x2d0) = arg3;
  (uint8_t)arg3 = 1;
  *(uint32_t*)((char*)r10 + 0x2c8) = arg4;
  return sub_fffff8000034cf20(arg1, r11, (uint8_t)arg3);
}

void sub_fffff8000034cf20(int32_t* arg1, int64_t* arg2, char arg3)
{
  if (arg3)
  {
    void* r8 = *(uint64_t*)(*(uint64_t*)arg2 + 0x140);
    int32_t r9_1 = *(uint32_t*)((char*)r8 + 0x278);
    
    if (r9_1 - 7 <= 0x1b)
      *(uint32_t*)((char*)r8 + 0x278) = r9_1 + 0x4d;
  }
  
  arg2[0x2c1] = 0x100;
  *(uint32_t*)arg1 = 0x15;
}


