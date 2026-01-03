import logging
import os

import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_build(dut: IdfDut) -> None:
    binary_file = os.path.join(dut.app.binary_path, 'cookie.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('blink_bin_size : {}KB'.format(bin_size // 1024))
