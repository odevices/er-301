import os
import sys

verbose = False
output_filename = sys.argv[1]
print("ramdisk.py: output =", output_filename)
root_dir = sys.argv[2]
print("ramdisk.py: root =", root_dir)

def write(text, file):
  if verbose: print(text)
  print(text, file=file)

names = {}
n = 0
with open(output_filename,"wt") as out:
  write(".global ramdisk_path_array",file=out)
  write(".global ramdisk_file_data_array",file=out)
  write(".global ramdisk_file_size_array",file=out)
  write(".global ramdisk_num",file=out)
  for dirpath, dnames, fnames in os.walk(root_dir):
    for f in fnames:
      if f.endswith("~"): continue
      path = os.path.join(dirpath,f)
      name = path.replace(root_dir+os.sep,"").replace(os.sep,"/")
      if verbose: print("ramdisk.py:", name)
      names[n] = name
      #write(".global %s"%name, file=out)
      write("file%d_data:"%n, file=out)
      write(".incbin \"%s\""%path.replace(os.sep,"/"), file=out)
      write("file%d_end:"%n, file=out)
      write(".byte 0", file=out)
      n += 1
  for e in range(n):
    write("path%d:"%e,file=out)
    write(".string \"%s\""%names[e],file=out)
  write("ramdisk_path_array:",file=out)
  for e in range(n):
    write(".word path%d"%e,file=out)
  write("ramdisk_file_data_array:",file=out)
  for e in range(n):
    write(".word file%d_data"%e,file=out)
  write("ramdisk_file_size_array:",file=out)
  for e in range(n):
    write(".word (file%d_end - file%d_data)"%(e,e), file=out)
  write("ramdisk_num:",file=out)
  write(".word %s"%hex(n),file=out)

print("ramdisk.py: Found",n,"files.")