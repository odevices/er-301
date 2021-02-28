import os
from invoke import Collection, task

ROOT_DIR = os.path.dirname(__file__)


@task
def clean(ctx):
    """"""
    with ctx.cd(ROOT_DIR):
        ctx.run("rm -f testlib.pkg")
    with ctx.cd(os.path.join(ROOT_DIR, "objects")):
        ctx.run("make clean")


@task
def build(ctx):
    """"""
    with ctx.cd(os.path.join(ROOT_DIR, "objects")):
        ctx.run("make")
    with ctx.cd(ROOT_DIR):
        print("ZIP testlib.pkg")
        ctx.run("zip -j testlib.pkg *.lua */testing/*.elf")
        ctx.run("unzip -l testlib.pkg")


@task(clean, build)
def rebuild(ctx):
    """"""
