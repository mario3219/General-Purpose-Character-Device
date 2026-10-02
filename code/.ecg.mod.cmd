savedcmd_ecg.mod := printf '%s\n'   ecg.o | awk '!x[$$0]++ { print("./"$$0) }' > ecg.mod
