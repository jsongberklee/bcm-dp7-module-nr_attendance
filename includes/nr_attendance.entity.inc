<?php
/**
 * Implements hook_entity_info().
 */
function nr_attendance_entity_info() {
  return array(
    'nr_attendance' => array(
      'label' => t('Node Registration Attendance'),
      'plural label' => t('Node Registration Attendances'),
      'entity class' => 'NRAttendanceEntityClass',
      'controller class' => 'NRAttendanceController',
      'base table' => 'nr_attendance',
      'entity keys' => array(
        'id' => 'attendance_id',
      ),
      // Use the default label() and uri() functions
      'uri callback' => 'nr_attendance_uri',
      'label callback' => 'nr_attendance_label',
    ),
  );
}
/**
 * URI callback for the Node registration type.
 */
function nr_attendance_uri($nrregistration) {
  //$registration_id = is_scalar($nrregistration) ? (int) $nrregistration : (int) $nrregistration->registration_id;
  //return array('path' => 'nr_attendance/' . $registration_id);
  return array('path' => 'node_registration_uri');
}

/**
 * Label callback for the Node registration type.
 */
function nr_attendance_label($nrregistration) {
  $label = $nrregistration->user_name;
  drupal_alter('nr_attendance_label', $label, $nrregistration);
  return $label;
  //return 'node_registration_label';
}

/**
 * Controller class
 */
class NRAttendanceController extends EntityAPIController {
}
/**
 * Entity class for Node Registrations
 */
class NRAttendanceEntityClass extends Entity {
}