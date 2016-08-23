<?php
/**
 * A handler to display dates as just the time if today, otherwise as time and date.
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_nr_attendance_date extends views_handler_field_date {

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['date_format']['#options']['nr_attendance'] = t('Attendance Status');
  }

  function render($values) {
    $format = $this->options['date_format'];
    $value = $values->{$this->field_alias};
    // check for our custom format
    if ($format == 'nr_attendance') {
      // check to see whether value is whithin today range
      $timezone = new DateTimeZone("America/New_York");
      $date = new DateTime();
      $date->setTimezone($timezone);
      $today = strtotime($date->format('Y-m-d 00:00:00'));
      if ($value >= $today) {
        // this value is still within today
        return format_date($value, 'custom', 'g:i A');
      }
      else {
        // this value is older than today
        return format_date($value, 'custom', 'j M Y g:i A');
      }
    }
    // otherwise render the date using the normal date handler
    else {
      return parent::render($values);
    }
  }

}