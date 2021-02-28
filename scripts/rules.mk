DEPFLAGS = -MF $@.d -MT $@ -MMD -MP

$(out_dir)/%.o: %.c
	@echo $(describe_env) C $(describe_input)
	@mkdir -p $(@D)
	@$(CC) $(DEPFLAGS) $(CFLAGS) -std=gnu11 -c $< -o $@

$(out_dir)/%.o: %.cpp
	@echo $(describe_env) C++ $(describe_input)
	@mkdir -p $(@D)
	@$(CPP) $(DEPFLAGS) $(CFLAGS) -std=gnu++11 -c $< -o $@

$(out_dir)/%.o: %.S
	@echo $(describe_env) ASM $(describe_input)
	@mkdir -p $(@D)
	@$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@

# SWIG
.PRECIOUS: $(out_dir)/%_swig.c $(out_dir)/%_swig.cpp

$(out_dir)/%_swig.c: %.c.swig
	@echo $(describe_env) "SWIG" $(describe_input)
	@mkdir -p $(@D)
	@$(SWIG) $(DEPFLAGS) $(SWIGFLAGS) -o $@ $<

$(out_dir)/%_swig.cpp: %.cpp.swig
	@echo $(describe_env) "SWIG++" $(describe_input)
	@mkdir -p $(@D)
	@$(SWIG) $(DEPFLAGS) -fvirtual -fcompact -c++ $(SWIGFLAGS) -o $@ $<

$(out_dir)/%_swig.o: $(out_dir)/%_swig.c
	@echo $(describe_env) "C(SWIG)" $(describe_input)
	@mkdir -p $(@D)
	@$(CC) $(DEPFLAGS) $(CFLAGS.swig) -std=gnu11 -c $< -o $@

$(out_dir)/%_swig.o: $(out_dir)/%_swig.cpp
	@echo $(describe_env) "C++(SWIG)" $(describe_input)
	@mkdir -p $(@D)
	@$(CPP) $(DEPFLAGS) $(CFLAGS.swig) -std=gnu++11 -c $< -o $@

# Rules for files generated in the build directory.

$(out_dir)/%.o: $(out_dir)/%.c
	@echo $(describe_env) C $(describe_input)
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -std=c11 -c $< -o $@

$(out_dir)/%.o: $(out_dir)/%.cpp
	@echo $(describe_env) C++ $(describe_input)
	@mkdir -p $(@D)
	@$(CPP) $(DEPFLAGS) $(CFLAGS) -std=gnu++11 -c $< -o $@

$(out_dir)/%.o: $(out_dir)/%.S
	@echo $(describe_env) ASM $(describe_input)
	@mkdir -p $(@D)
	@$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@

# Dependencies

#DEPFILES = $(objects:%.o=%o.d)
DEPFILES := $(call rwildcard,$(out_dir),*.d)
$(DEPFILES): ;
-include $(DEPFILES)