#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

REMOTE_HOST="marley"
REMOTE_DIR="/home/ubuntu/net/ethanjalexander"
SERVICE_NAME="ethanjalexander"

rsync -avz --delete --chmod=F644,D755 \
    assets src include Makefile \
    "$REMOTE_HOST:$REMOTE_DIR/"

rsync -avz "scripts/${SERVICE_NAME}.service" "$REMOTE_HOST:/tmp/${SERVICE_NAME}.service"
rsync -avz "scripts/${SERVICE_NAME}.nginx" "$REMOTE_HOST:/tmp/${SERVICE_NAME}.nginx"

ssh "$REMOTE_HOST" REMOTE_DIR="$REMOTE_DIR" SERVICE_NAME="$SERVICE_NAME" bash -s <<'EOF'
set -euo pipefail

cd "$REMOTE_DIR"
mkdir -p data

make

sudo install -m 644 "/tmp/${SERVICE_NAME}.service" "/etc/systemd/system/${SERVICE_NAME}.service"
sudo install -m 644 "/tmp/${SERVICE_NAME}.nginx" "/etc/nginx/sites-enabled/ethanjalexander"
rm -f "/tmp/${SERVICE_NAME}.service" "/tmp/${SERVICE_NAME}.nginx"

sudo systemctl daemon-reload
sudo systemctl enable "${SERVICE_NAME}"
sudo systemctl restart "${SERVICE_NAME}"

sudo nginx -t
sudo systemctl reload nginx
EOF

echo "Deployed to https://ethanjalexander.ca"
