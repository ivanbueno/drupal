<?php

/**
 * @file
 * Definition of Drupal\epm\Plugin\views\field\NodeTicketResolutionTime
 */

namespace Drupal\epm\Plugin\views\field;

use Drupal\views\Plugin\views\field\FieldPluginBase;
use Drupal\views\ResultRow;

/**
 * Field handler to display the ticket resolution time.
 *
 * @ingroup views_field_handlers
 *
 * @ViewsField("node_ticket_resolution_time")
 */
class NodeTicketResolutionTime extends FieldPluginBase {

    /**
     * @{inheritdoc}
     */
    public function query() {
    }

    /**
     * @{inheritdoc}
     */
    public function render(ResultRow $values) {
        /* @var \Drupal\epm\Service\Techhub $techhub */
        $techhub = \Drupal::service('epm.techhub');
        $resolution_time = $techhub->getResolutionTime($values->nid);

        return \Drupal::service('date.formatter')->formatInterval($resolution_time['total'], $granularity = 2);
    }
}