<?php
/**
 * Implements hook_action_info().
 */
function nr_attendance_action_info() {
  return array(
    // Formless actions.
    'nr_attendance_confirm_action' => array(
      'type' => 'nr_attendance',
      'label' => t('Confirmed'),
      'configurable' => FALSE,
      'behavior' => array('changes_property'),
      'triggers' => array(''),
    ),
    'nr_attendance_unconfirm_action' => array(
      'type' => 'nr_attendance',
      'label' => t('(UN) Confirm'),
      'configurable' => FALSE,
      'behavior' => array('changes_property'),
      'triggers' => array(''),
    ),
    'nr_attendance_bypass_action' => array(
      'type' => 'nr_attendance',
      'label' => t('Bypass'),
      'configurable' => FALSE,
      'behavior' => array('changes_property'),
      'triggers' => array(''),
    ),
    'nr_attendance_unbypass_action' => array(
      'type' => 'nr_attendance',
      'label' => t('(UN) Bypass'),
      'configurable' => FALSE,
      'behavior' => array('changes_property'),
      'triggers' => array(''),
    ),
    'nr_attendance_attended_action' => array(
      'type' => 'nr_attendance',
      'label' => t('Attended'),
      'configurable' => FALSE,
      'behavior' => array('changes_property'),
      'triggers' => array(''),
    ),
    'nr_attendance_unattended_action' => array(
      'type' => 'nr_attendance',
      'label' => t('Absent'),
      'configurable' => FALSE,
      'behavior' => array('changes_property'),
      'triggers' => array(''),
    ),
  );

/*
    return array(
    // Formless actions.
    'nr_attendance_confirm_action' => array(
      'type' => 'nr_attendance',
      'label' => t('Confirm YES attendance'),
      'configurable' => FALSE,
      'behavior' => array('changes_property'),
      'triggers' => array(''),
    ),
    'nr_attendance_unconfirm_action' => array(
      'type' => 'nr_attendance',
      'label' => t('Confirm NO attendance'),
      'configurable' => FALSE,
      'behavior' => array('changes_property'),
      'triggers' => array(''),
    ),
    'nr_attendance_bypass_action' => array(
      'type' => 'nr_attendance',
      'label' => t('Bypass YES attendance'),
      'configurable' => FALSE,
      'behavior' => array('changes_property'),
      'triggers' => array(''),
    ),
    'nr_attendance_unbypass_action' => array(
      'type' => 'nr_attendance',
      'label' => t('Bypass No attendance'),
      'configurable' => FALSE,
      'behavior' => array('changes_property'),
      'triggers' => array(''),
    ),
    'nr_attendance_attended_action' => array(
      'type' => 'nr_attendance',
      'label' => t('Attended YES attendance'),
      'configurable' => FALSE,
      'behavior' => array('changes_property'),
      'triggers' => array(''),
    ),
    'nr_attendance_unattended_action' => array(
      'type' => 'nr_attendance',
      'label' => t('Attended No attendance'),
      'configurable' => FALSE,
      'behavior' => array('changes_property'),
      'triggers' => array(''),
    ),
  );
*/

}

function nr_attendance_confirm_action(&$nrregistration, $context) {
	_attendance_action_update('confirm', $nrregistration);
}
function nr_attendance_unconfirm_action(&$nrregistration, $context) {
	_attendance_action_update('confirm', $nrregistration, false);
}
function nr_attendance_bypass_action(&$nrregistration, $context) {
	_attendance_action_update('bypass', $nrregistration);
}
function nr_attendance_unbypass_action(&$nrregistration, $context) {
	_attendance_action_update('bypass', $nrregistration, false);
}
function nr_attendance_attended_action(&$nrregistration, $context) {
	_attendance_action_update('attend', $nrregistration);
	// confirmed field to be set automatically when trainer check the attendance
	_attendance_action_update('confirm', $nrregistration);
}
function nr_attendance_unattended_action(&$nrregistration, $context) {
	_attendance_action_update('attend', $nrregistration, false);
	// confirmed field to be set automatically when trainer check the attendance
	_attendance_action_update('confirm', $nrregistration);
}