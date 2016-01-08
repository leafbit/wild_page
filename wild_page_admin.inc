<?php

/**
 * List wild page definitions.
 */
function wild_page_admin(){

  $header = array(
    'title' => t('Title'),
    'path' => t('Path'),
    'page_callback' => t('Page Callback'),
    'actions' => t('Actions'),
  );

  $qh = db_select('wild_page', 'm')
    ->fields('m', array('title', 'path', 'page_callback', 'wmid'))
    ->orderBy('title')
    ->execute();

  $rows = array();
  $link_params = array(
    'query' => array(
      'destination' => current_path(),
    ),
  );
  while ($row = $qh->fetch(PDO::FETCH_ASSOC)) {
    $links = array();
    $links[] = l(t('Edit'), 'admin/structure/wild_page/'
      . $row['wmid'] . '/edit', $link_params);
    $links[] = l(t('Delete'), 'admin/structure/wild_page/'
      . $row['wmid'] . '/delete');

    unset($row['wmid']);
    $row['actions'] = implode(' ', $links);
    $rows[] = $row;
  }

  $build['add_new_wild_page'] = array(
    '#markup' => l(t('Add page'), 'admin/structure/wild_page/add', $link_params),
  );

  $build['wild_pages'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No pages defined yet'),
  );

  return $build;
}

/**
 * Wild page forms wrapper.
 */
function wild_page_wrapper($mode, $wild_page) {
  if ($mode == 'edit') {
    return drupal_get_form('wild_page_admin_form', $wild_page);
  }
  elseif ($mode == 'delete') {
    return drupal_get_form('wild_page_admin_delete_form', $wild_page);
  }
  else {
    return 'Unknown mode';
  }
}

/**
 * Wild page edit form.
 */
function wild_page_admin_form($form, $form_state, $wm) {

  _wild_page_admin_include_modal();

  $php_default = file_get_contents(drupal_get_path('module', 'wild_page')
    . '/wild_page_template.inc');
  //dpm($wm);
  $v = array(
    'wmid' => FALSE,
    'mlid' => FALSE,
    'path' => '',
    'title' => '',
    'type' => MENU_NORMAL_ITEM,
    'access_callback' => 'user_access',
    'access_arguments' => 'eck entitytype bundle edit entities',
    'page_callback' => 'wild_page_view_page',
    'data' => $php_default,
    'disabled' => 0,
  );
  if ($wm)  $v = (array) $wm;


  $form['mlid'] = array(
    '#type' => 'hidden',
    '#default_value' => $v['mlid'],
  );

  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#size' => 20,
    '#required' => TRUE,
    '#default_value' => $v['title'],
  );
  $form['path'] = array(
    '#title' => t('Path'),
    '#type' => 'textfield',
    '#size' => 50,
    '#required' => TRUE,
    '#default_value' => $v['path'],
    '#description' => t('Menu definition path. Can include % wild chars.'),
  );

  $options = array();
  foreach (menu_get_names() as $nm) $options[$nm] = $nm;

  $menu_link = ($v['mlid']) ? (object) menu_link_load($v['mlid'])
    : (object) array(
    'plid' => 0,
    'menu_name' => 'navigation',
  );

  $form['menu_name'] = array(
    '#type' => 'select',
    '#title' => t('Menu Name'),
    '#options' => $options,
    '#default_value' => $menu_link->menu_name,
    '#description' => t('The custom menu under which shall place the link'),
  );

  $options = array(
    MENU_NORMAL_ITEM => 'MENU_NORMAL_ITEM',
    MENU_CALLBACK => 'MENU_CALLBACK',
    MENU_LOCAL_ACTION => 'MENU_LOCAL_ACTION',
    MENU_LOCAL_TASK => 'MENU_LOCAL_TASK',
    MENU_DEFAULT_LOCAL_TASK => 'MENU_DEFAULT_LOCAL_TASK',
  );

  $form['type'] = array(
    '#type' => 'select',
    '#title' => t('Menu Type'),
    '#options' => $options,
    '#default_value' => $v['type'],
    '#description' => t('Description'),
  );
  /* So far will limit to user_access callback
  $form['access_callback'] = array(
    '#title' => t('Access callback'),
    '#type' => 'textfield',
    '#size' => 30,
    '#required' => FALSE,
    '#default_value' => $v['access_callback'],
    '#description' => t('If ommited user_access callback will be used.'),
  );
   */
  // Render permission overview:
  $options = array();
  foreach (module_list(FALSE, FALSE, TRUE) as $module) {
    if ($permissions = module_invoke($module, 'permission')) {
      // List only the modules that have permissions.
      foreach($permissions as $pk=>$pm) $options[$pk] = $pm['title'];
    }
  }
  $form['access_arguments'] = array(
    '#title' => t('Access arguments'),
    '#type' => 'textfield',
    '#size' => 60,
    '#required' => FALSE,
    '#default_value' => $v['access_arguments'],
    '#description' => t('Arguments that shall be passed to access function. Integers may be used to point at path parts values.'),
  );

  $form['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Menu link weight'),
    '#size' => 4,
    '#default_value' => $v['weight'],
  );

  $form['access_arguments'] = array(
    '#type' => 'select',
    '#title' => t('Access arguments'),
    '#options' => $options,
    '#default_value' => $v['access_arguments'],
    '#description' => t('Description'),
  );
  /* So far - will limit to wild_page_page_view callback.
  $form['page_callback'] = array(
    '#title' => t('Page callback'),
    '#type' => 'textfield',
    '#size' => 30,
    '#required' => FALSE,
    '#default_value' => $v['page_callback'],
    '#description' => t('If ommited wild_page_page_view callback will be used.'),
  );
   */
  $btn = ctools_modal_text_button(t('Snippets'), 'wild_page/nojs/snippets', t('Snippets'));
  $form['data'] = array(
    '#title' => t('PHP code') . ' ' . $btn,
    '#type' => 'textarea',
    '#default_value' => $v['data'],
    '#rows' => 30,
    '#required' => TRUE,
    '#description' => 'PHP code that builds the page. Include <' . '?php and ?>',
  );

  $form['disabled'] = array(
    '#title' => t('Disabled'),
    '#type' => 'checkbox',
    '#default_value' => $v['disabled'],
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Saves form submission for wild pages.
 */
function wild_page_admin_form_submit($form, $form_state){
  $v = $form_state['values'];
  $o = $form_state['build_info']['args'][0];
  $v['wmid'] = $o ? $o->wmid : FALSE;
  wild_page_save($v);
}

/**
 * Form used to delete wild page definition.
 */
function wild_page_admin_delete_form($form, $form_state, $wm) {
  $form = confirm_form($form, t('Are you shure you want to delete?'),
    'admin/structure/wild_page');
  return $form;
}

/**
 * Handles the submission of form to delete wild page definition.
 */
function wild_page_admin_delete_form_submit($form, $form_state){
  $o = $form_state['build_info']['args'][0];
  $v = $form_state['values'];
  //if ($o->mlid)  page_link_delete($o->mlid);
  if ($v['op'] = t('Confirm')) {
    wild_page_delete($o->wmid);
  }

  menu_rebuild();
  menu_cache_clear_all();
  drupal_goto('admin/config/wild_page');
}

/**
 * Modal callback - snippets selector.
 */
function wild_page_admin_snippets($js = NULL){
  if ($js) {
    ctools_include('modal');
    ctools_include('ajax');
  }

  $form_state = array(
    'ajax' => $js,
  );

  if ($js) {
    $output = ctools_modal_form_wrapper('wild_page_admin_snippets_form', $form_state);
    if ($form_state['submitted']) {
      $op = $form_state['values']['snippet'];
      $path = drupal_get_path('module', 'wild_page')
        . '/snippets/' . $op . '.inc';
      $content = file_get_contents($path);
      $output = array(
        ajax_command_invoke('#edit-data', 'val', array($content)),
        ctools_modal_command_dismiss(),
      );
    }
    print ajax_render($output);
    drupal_exit();
  }
}

function wild_page_admin_snippets_form($form, $state){

  $path = drupal_get_path('module', 'wild_page') . '/snippets';

  $files = file_scan_directory($path, '/.inc$/');

  $options = array();
  foreach($files as $f) {
    $options[$f->name] = ucfirst(preg_replace("/\W|\_/", ' ', $f->name));
  }

  $form['snippet'] = array(
    '#type' => 'radios',
    '#options' => $options,
    '#default_value' => 'detect_mobile', //current(array_keys($options)),
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Insert'),
  );

  return $form;
}
// Utils and other.

function _wild_page_admin_include_modal() {

  static $added = FALSE;
  if ($added == FALSE) {
    $added = TRUE;

    // Include the CTools tools that we need.
    ctools_include('modal');
    ctools_include('ajax');
    ctools_modal_add_js();

    $example_style = array(
      'example-contact-style' => array(
        'modalSize' => array(
          'type' => 'fixed',
          'width' => 120,
          'height' => 'auto',
        ),
        'modalOptions' => array(
          'opacity' => (float) 0.3,
          'background-color' => '#000000',
        ),
        'closeText' => '',
        'loadingText' => '',
        'animation' => 'fadeIn',
        'animationSpeed' => 'fast',
      ),
    );

    drupal_add_js($example_style, 'setting');
  }
}

