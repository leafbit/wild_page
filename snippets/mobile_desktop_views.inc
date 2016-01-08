<?php
// Displays a view or another based on computer type that
// accesses the site - desktop/tablet or mobile phone.

// Detect if is mobile phone or tablet.
$is_mobile = FALSE;
if (module_exists('mobile_detect')) {
  $detect = mobile_detect_get_object();
  if ($detect) {
    $is_mobile = $detect->isMobile() && !$detect->isTablet();
  }
}

// Decide what view to show:
if ($is_mobile) {
  $view_name = 'mobile_view_machine_name';
  $display_name = 'display_name';
}
else {
  $view_name = 'desktop_view_machine_name';
  $display_name = 'display_name';
}

// Prepare and display view:
$view = views_get_view($view_name);
if (is_object($view)) {
  if ($display_name) $view->set_display($display_name);
  // $view->set_arguments(array($foo));
  $view->override_path = current_path();
  $view->execute();
  print $view->render($display_name ? $display_name : NULL);
}


// or
// echo views_embed_view($view_name, $display_name, array());
?>
