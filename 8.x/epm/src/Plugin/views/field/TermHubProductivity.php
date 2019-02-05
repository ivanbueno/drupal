<?php

/**
 * @file
 * Definition of Drupal\epm\Plugin\views\field\TermHubProductivity
 */

namespace Drupal\epm\Plugin\views\field;

use Drupal\Core\Form\FormStateInterface;
use Drupal\views\Plugin\views\field\FieldPluginBase;
use Drupal\views\ResultRow;

/**
 * Field handler to display the hub productivity.
 *
 * @ingroup views_field_handlers
 *
 * @ViewsField("term_hub_productivity")
 */
class TermHubProductivity extends FieldPluginBase {


    /**
     * Define the available options
     * @return array
     */
    protected function defineOptions() {
        $options = parent::defineOptions();
        $options['productivity_type'] = array('default' => 'nps');

        return $options;
    }

    /**
     * Provide the options form.
     */
    public function buildOptionsForm(&$form, FormStateInterface $form_state) {
        $form['productivity_type'] = array(
            '#title' => $this->t('Productivity value'),
            '#type' => 'select',
            '#default_value' => $this->options['productivity_type'],
            '#options' => [
                'time' => t('Time'),
                'money' => t('Money'),
                'nps' => t('NPS'),
            ],
        );

        parent::buildOptionsForm($form, $form_state);
    }

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

        switch ($this->options['productivity_type']) {
            case 'time':
                $productivity = $techhub->getProductivity([$values->tid]);
                return "{$productivity['time']}";
                break;

            case 'money':
                $productivity = $techhub->getProductivity([$values->tid]);
                return "\${$productivity['money']}";
                break;

            case 'nps':
            default:
                $nps = $techhub->getFeedbackNps([$values->tid]);
                return round($nps['score']);
                break;
        }
    }
}