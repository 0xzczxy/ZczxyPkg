Would this function work better to patch?


```c
{
    int16_t var_60 = 0;
    int64_t var_48 = 0;
    void* var_50 = nullptr;
    int128_t var_80;
    __builtin_memset(&var_80, 0, 0x18);
    RtlImageNtHeaderEx(0, arg2, (uint64_t)arg3, &var_50);
    
    if (*(uint16_t*)((char*)var_50 + 4) != 0x8664)
        return 0xc00000bb;
    
    int64_t var_98 = 0;
    int64_t* var_a8 = nullptr;
    uint64_t r13;
    (uint8_t)r13 = 1;
    int32_t var_b4 = 0xffffffff;
    uint128_t* r15_1 = nullptr;
    char var_b8 = 1;
    int64_t* r14_1 = nullptr;
    int32_t rdi_1 = -1;
    
    if (!TEST_BITD(arg4, 0xc))
    {
        int32_t rax_2 = BlGetBootOptionBoolean((char*)arg1 + 0x2c, 0x16000060, &var_b8);
        r13 = (uint64_t)var_b8;
        
        if (rax_2 < 0)
            r13 = 0;
    }
    
    int128_t var_90 = {0};
    int32_t rax_3;
    int32_t rcx_1;
    rax_3 = BlArchCpuId(7, 0, &var_90);
    int32_t rsi_1;
    int64_t var_a0;
    int64_t r12;
    
    if (!(*(uint32_t*)((char*)var_90)[8] & 0x10000))
    {
        (uint8_t)r12 = 0;
    label_101ab105:
        (uint8_t)rax_3 = (uint8_t)r12;
        ArchFiveLevelPaging = (uint8_t)r12;
        int32_t rax_8 = 0x4000;
        ArchPagingLevels = -((rcx_1 - rcx_1)) + 4;
        int64_t rbx_2;
        
        while (true)
        {
            int32_t var_b0_1 = rax_8;
            int32_t rax_9 = rax_8 + 0x24000;
            int32_t var_58_1 = rax_9;
            
            if ((uint8_t)r13)
                rax_9 += 0x1080;
            
            uint64_t rax_11 = (uint64_t)(rax_9 + 0xfff) & 0xfffff000;
            var_a0 = rax_11;
            int32_t rax_12 = BlMmAllocatePhysicalPagesInRange(&var_98, 
                (uint64_t)(uint32_t)rax_11 >> 0xc, 0xd0000009, 0, 0, nullptr, 0);
            rbx_2 = var_98;
            rsi_1 = rax_12;
            
            if (rax_12 < 0)
                break;
            
            int64_t r15_2 = var_a0;
            uint128_t* var_40 = rbx_2;
            rsi_1 = BlMmMapPhysicalAddressEx(&var_40, rbx_2, r15_2, 0x40000, 0);
            
            if (rsi_1 < 0)
            {
                r15_1 = nullptr;
                break;
            }
            
            r15_1 = var_40;
            uint64_t r8_6 = memset(r15_1, 0, r15_2);
            (uint64_t)var_90 = &r15_1[0x2400];
            *(uint32_t*)((char*)var_90)[0xc] = var_b0_1;
            *(uint32_t*)((char*)var_90)[8] = var_b0_1;
            
            if (!(uint8_t)r13)
                BlpArchGetDescriptorTableContext(&var_80);
            else
            {
                void* rdi_3 = (uint64_t)var_58_1 + r15_1;
                int32_t var_c8_1 = 0;
                int32_t var_c8_2 = 1;
                int32_t var_c8_3 = 2;
                int32_t var_c8_4 = 2;
                ArchSetGdtEntry(rdi_3, 0x30, 
                    ArchSetGdtEntry(rdi_3, 0x20, 
                        ArchSetGdtEntry(rdi_3, 0x10, 
                            ArchSetGdtEntry(rdi_3, 0, r8_6, 0, 0), 0, 0x1b), 
                        0xffffffff, 0x1b), 
                    0xffffffff, 0x13);
                *(uint64_t*)((char*)var_80)[2] = rdi_3;
                (uint16_t)var_80 = 0x7f;
                *(uint16_t*)((char*)var_80)[0xa] = 0xfff;
                *(uint64_t*)((char*)var_80)[0xc] = (char*)rdi_3 + 0x80;
                int64_t var_70_1;
                *(uint32_t*)((char*)var_70_1)[4] = 0x100000;
                *(uint32_t*)((char*)var_70_1)[8] = 0x300030;
                *(uint32_t*)((char*)var_70_1)[0xc] = 0x300030;
                int16_t var_60_1 = 0;
                char rax_16;
                rax_16 = BlBdDebugTransitionsEnabled();
                
                if (rax_16)
                {
                    r8_6 = (uint64_t)*(uint16_t*)((char*)var_70_1)[6];
                    BlBdPatchIdt(*(uint64_t*)((char*)var_80)[0xc], 
                        (uint32_t)*(uint16_t*)((char*)var_80)[0xa], (uint16_t)r8_6);
                }
            }
            
            int32_t rax_17 = BlpArchBuildx64PageTable(&var_b4, &var_a8, r8_6, 
                *(uint8_t*)((char*)arg1 + 0xa) & 1);
            rsi_1 = rax_17;
            
            if (rax_17 >= 0)
            {
                int32_t rax_18 =
                    ImgpInitializeBootApplicationParameters(&var_90, arg1, arg2, arg3);
                rsi_1 = rax_18;
                
                if (rax_18 == 0xc0000023)
                {
                    r14_1 = var_a8;
                    rdi_1 = var_b4;
                    
                    if (r14_1)
                    {
                        if (rdi_1 != 0xffffffff)
                        {
                            r14_1[0x1ed] &= 0xfffffffffffffffe;
                            ArchpFreex64PageTableLevel(rdi_1, r14_1, ArchPagingLevels);
                        }
                        
                        r14_1 = nullptr;
                        var_a8 = nullptr;
                    }
                    
                    if (r15_1)
                    {
                        BlMmUnmapVirtualAddressEx(r15_1, var_a0);
                        r15_1 = nullptr;
                    }
                    
                    if (rbx_2)
                    {
                        BlMmFreePhysicalPages(rbx_2);
                        rbx_2 = 0;
                        var_98 = 0;
                    }
                    
                    rax_8 = (*(uint32_t*)((char*)var_90)[0xc] + 0xfff) & 0xfffff000;
                    *(uint32_t*)((char*)var_90)[0xc] = rax_8;
                    
                    if (var_b0_1 < rax_8)
                        continue;
                    
                    rsi_1 = -0x3ffffdfa;
                    break;
                }
                else if (rsi_1 >= 0)
                {
                    void* rax_21 = var_50;
                    void* rdi_5 = (uint64_t)var_90;
                    *(uint16_t*)((char*)rax_21 + 0x18);
                    r14_1 = var_a8;
                    rsi_1 = BlpArchTransferTo64BitApplication(
                        (uint64_t)*(uint32_t*)((char*)rax_21 + 0x28) + arg2, rdi_5, 
                        &r15_1[0x2400], r14_1, (uint8_t)r12, &var_80);
                    int32_t rax_24 = BlMmSynchronizeMemoryMap();
                    
                    if (rax_24 < 0)
                        rsi_1 = rax_24;
                    
                    int128_t* rax_25 = (uint64_t)*(uint32_t*)((char*)rdi_5 + 0x34);
                    *(uint128_t*)arg5 = *(uint128_t*)((char*)rax_25 + rdi_5);
                    arg5[1] = *(uint64_t*)((char*)rax_25 + rdi_5 + 0x10);
                    rdi_1 = var_b4;
                    *(uint32_t*)((char*)arg5 + 0x18) =
                        *(uint32_t*)((char*)rax_25 + rdi_5 + 0x18);
                    break;
                }
            }
            
            rdi_1 = var_b4;
            r14_1 = var_a8;
            break;
        }
        
        if (r14_1 && rdi_1 != 0xffffffff)
        {
            r14_1[0x1ed] &= 0xfffffffffffffffe;
            ArchpFreex64PageTableLevel(rdi_1, r14_1, ArchPagingLevels);
        }
        
        if (r15_1)
            BlMmUnmapVirtualAddressEx(r15_1, var_a0);
        
        if (rbx_2)
            BlMmFreePhysicalPages(rbx_2);
    }
    else
    {
        char rax_4 = BlArchIsFiveLevelPagingActive();
        (uint8_t)r12 = rax_4;
        
        if (rax_4)
            goto label_101ab05c;
        
        if (BlGetBootOptionInteger((char*)arg1 + 0x2c, 0x15000088, &var_48) < 0
                || var_48 != 2)
            goto label_101ab05c;
        
        var_a0 = 0;
        int32_t rax_6 = BlpArchAllocatePagingLevelChangeRoutineBuffer(&var_a0);
        int64_t rbx_1 = var_a0;
        rsi_1 = rax_6;
        int32_t rax_7;
        
        if (rax_6 >= 0)
        {
            rax_7 = ArchInitializePagingLevelChangeRoutineVector(rbx_1, 0x2000);
            rsi_1 = rax_7;
        }
        
        if ((rax_6 < 0 || rax_7 < 0) && rbx_1)
        {
            BlMmUnmapVirtualAddressEx(rbx_1, 0x2000);
            BlMmFreePhysicalPages(rbx_1);
        }
        
        if (rsi_1 >= 0)
        {
            (uint8_t)r12 = 1;
        label_101ab05c:
            rax_3 = BlpArchGetFiveLevelMode(1);
            
            if (!(uint8_t)r12)
            {
                if (!(uint8_t)rax_3)
                    goto label_101ab105;
                
                rsi_1 = -0x3ffffff3;
            }
            else
            {
                if ((uint8_t)rax_3)
                    goto label_101ab105;
                
                rax_3 = BlpArchAllocatePagingLevelChangeRoutineBuffer(
                    &IapSwitchPagingLevels);
                rsi_1 = rax_3;
                
                if (rax_3 >= 0)
                    goto label_101ab105;
            }
        }
    }
    int64_t ArchpSwitchPagingLevelVector_1 = ArchpSwitchPagingLevelVector;
    
    if (ArchpSwitchPagingLevelVector_1)
    {
        ArchpSwitchPagingLevelVector = 0;
        BlMmUnmapVirtualAddressEx(ArchpSwitchPagingLevelVector_1, 0x2000);
        BlMmFreePhysicalPages(ArchpSwitchPagingLevelVector_1);
    }
    
    int64_t IapSwitchPagingLevels_1 = IapSwitchPagingLevels;
    
    if (IapSwitchPagingLevels_1)
    {
        BlMmUnmapVirtualAddressEx(IapSwitchPagingLevels_1, 0x2000);
        BlMmFreePhysicalPages(IapSwitchPagingLevels_1);
        IapSwitchPagingLevels = 0;
    }
    
    return (uint64_t)rsi_1;
}
```
 
