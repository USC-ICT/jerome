#!/bin/sh

#  copy_headers.sh
#  jerome
#
#  Created by Anton Leuski on 4/20/16.
#  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.

## copy public header files preserving directory structure

cd "${SRCROOT}/jerome"
mkdir -p "${BUILT_PRODUCTS_DIR}/include/${PRODUCT_NAME}"
rsync  -Cavm --filter="+ */" \
  --filter="- *.private.h" \
  --filter="+ *.hpp" \
  --filter="+ *.h" \
  --filter="- *"  \
. "${BUILT_PRODUCTS_DIR}/include/${PRODUCT_NAME}"
