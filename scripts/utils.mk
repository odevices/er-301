# A recursive version of the wildcard function.
# $(call rwildcard,directory,pattern)
export rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
