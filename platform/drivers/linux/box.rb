from "registry.test.pensando.io:5000/pensando/drivers-linux:0.2"

copy "platform/drivers/linux/entrypoint.sh", "/"
run "chmod +x /entrypoint.sh"

entrypoint "/entrypoint.sh"
