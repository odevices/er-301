import os
from invoke import Collection, task

ROOT_DIR = os.path.dirname(__file__)

@task
def clean(ctx):
    """"""
    with ctx.cd(os.path.join(ROOT_DIR)):
        ctx.run("make clean")

@task
def build(ctx):
    """"""
    with ctx.cd(os.path.join(ROOT_DIR)):
        ctx.run("make")

@task(clean)
def rebuild(ctx):
    """"""
    with ctx.cd(os.path.join(ROOT_DIR)):
        ctx.run("make")


@task
def install(ctx):
    """"""
    with ctx.cd(os.path.join(ROOT_DIR)):
        ctx.run("make install")

@task
def readelf(ctx):
    """"""
    with ctx.cd(os.path.join(ROOT_DIR,"testing")):
        ctx.run("readelf -a test.elf")