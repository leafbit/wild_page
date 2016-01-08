<?php
$view_name = 'view_machine_name';
$display_name = 'display_name';

$view = views_get_view($view_name);
if (is_object($my_view)) {
  $view->set_display($display_name);
  $view->set_arguments(array($foo));
  $view->pre_execute();
  print $view->render($my_display_name);
}

// or
// echo views_embed_view('view_machine_name', 'default');
?>
