@echo off
call setup\setup.cmd
if defined PY (
    echo Start QHL Approximation Tool with arguments:
    echo %*
    %PY% src\qhcg.py %*
)
