<?php

/**
 * Implements hook_views_data_alter().
 * @param array $data
 */
function epm_views_data_alter(array &$data) {
    $data['node']['node_ticket_resolution_time'] = [
        'title' => t('Ticket Resolution Time'),
        'group' => t('TechHub'),
        'field' => [
            'title' => t('Ticket Resolution Time'),
            'help' => t('Displays the resolution time of the ticket.'),
            'id' => 'node_ticket_resolution_time',
        ],
    ];
    $data['taxonomy_term_field_data']['term_hub_productivity'] = [
        'title' => t('Hub Productivity'),
        'group' => t('TechHub'),
        'field' => [
            'title' => t('Hub Productivity'),
            'help' => t('Displays the productivity stats of a hub.'),
            'id' => 'term_hub_productivity',
        ],
    ];
}