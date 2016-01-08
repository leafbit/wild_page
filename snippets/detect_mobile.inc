<?php
// Detect if is mobile phone or tablet/desktop.
$is_mobile = FALSE;
if (module_exists('mobile_detect')) {
  $detect = mobile_detect_get_object();
  if ($detect) {
    $is_mobile = $detect->isMobile() && !$detect->isTablet();
  }
}
?>
