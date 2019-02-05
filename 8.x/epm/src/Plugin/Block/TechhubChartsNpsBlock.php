<?php

namespace Drupal\epm\Plugin\Block;

use Drupal\Core\Block\BlockBase;
use Drupal\Core\Form\FormStateInterface;

\Drupal::moduleHandler()->loadInclude('charts', 'inc', 'includes/charts.pages');

/**
 * Provides a 'Techhub Charts NPS' Block.
 *
 * @Block(
 *   id = "techhub_charts_nps_block",
 *   admin_label = @Translation("Techhub - Charts - NPS"),
 *   category = @Translation("Techhub"),
 * )
 */
class TechhubChartsNpsBlock extends BlockBase {

    /**
     * {@inheritdoc}
     */
    public function defaultConfiguration() {

        // Get the default chart values.
        $defaults = \Drupal::state()->get('charts_default_settings', []);

        $defaults += charts_default_settings();
        foreach ($defaults as $default_key => $default_value) {
            $options[$default_key]['default'] = $default_value;
        }

        return $defaults;

    }

    /**
     * {@inheritdoc}
     */
    public function blockForm($form, FormStateInterface $form_state) {
        parent::blockForm($form, $form_state);

        // Merge in the global chart settings form.
        $field_options = [];
        $parents = ['charts_default_settings'];
        $form = charts_settings_form($form, $this->defaultConfiguration(), $field_options, $parents);

        /**
         * @todo figure out why the block label field does not respect weight.
         */
        // Reposition the block form fields to the top.
        $form['label']['#weight'] = '-26';
        $form['label_display']['#weight'] = '-25';

        /**
         * Unset the #parents element from default form, then set the
         * default value.
         */
        unset($form['library']['#parents']);
        $form['library']['#default_value'] = $this->configuration['library'];
        $form['library']['#weight'] = '-23';

        unset($form['type']['#parents']);
        $form['type']['#default_value'] = $this->configuration['type'];
        $form['type']['#weight'] = '-24';

        unset($form['display']['title']['#parents']);
        $form['display']['title']['#default_value'] = $this->configuration['title'];

        unset($form['display']['title_position']['#parents']);
        $form['display']['title_position']['#default_value'] = $this->configuration['title_position'];

        unset($form['display']['data_labels']['#parents']);
        $form['display']['data_labels']['#default_value'] = $this->configuration['data_labels'];

        unset($form['display']['background']['#parents']);
        $form['display']['background']['#default_value'] = $this->configuration['background'];

        unset($form['display']['legend_position']['#parents']);
        $form['display']['legend_position']['#default_value'] = $this->configuration['legend_position'];

        unset($form['display']['tooltips']['#parents']);
        $form['display']['tooltips']['#default_value'] = $this->configuration['tooltips'];

        unset($form['display']['dimensions']['height']['#parents']);
        $form['display']['dimensions']['height']['#default_value'] = $this->configuration['height'];

        unset($form['display']['dimensions']['width']['#parents']);
        $form['display']['dimensions']['width']['#default_value'] = $this->configuration['width'];

        /**
         * @todo: complete this for the remaining form elements.
         */

        return $form;
    }

    /**
     * {@inheritdoc}
     */
    public function blockSubmit($form, FormStateInterface $form_state) {

        $this->configuration['library'] = $form_state->getValue('library');
        $this->configuration['type'] = $form_state->getValue('type');
        $this->configuration['field_colors'] = $form_state->getValue('field_colors');
        $this->configuration['title'] = $form_state->getValue(['display','title']);
        $this->configuration['title_position'] = $form_state->getValue(['display','title_position']);
        $this->configuration['data_labels'] = $form_state->getValue(['display','data_labels']);
        $this->configuration['legend'] = $form_state->getValue('legend');
        $this->configuration['legend_position'] = $form_state->getValue(['display','legend_position']);
        $this->configuration['colors'] = $form_state->getValue('colors');
        $this->configuration['background'] = $form_state->getValue(['display','background']);
        $this->configuration['tooltips'] = $form_state->getValue(['display','tooltips']);
        $this->configuration['tooltips_use_html'] = $form_state->getValue('tooltips_use_html');
        $this->configuration['width'] = $form_state->getValue(['display','dimensions','width']);
        $this->configuration['height'] = $form_state->getValue(['display','dimensions','height']);
        $this->configuration['xaxis_title'] = $form_state->getValue('xaxis_title');
        $this->configuration['xaxis_labels_rotation'] = $form_state->getValue('xaxis_labels_rotation');
        $this->configuration['yaxis_title'] = $form_state->getValue('yaxis_title');
        $this->configuration['yaxis_min'] = $form_state->getValue('yaxis_min');
        $this->configuration['yaxis_max'] = $form_state->getValue('yaxis_max');
        $this->configuration['yaxis_prefix'] = $form_state->getValue('yaxis_prefix');
        $this->configuration['yaxis_suffix'] = $form_state->getValue('yaxis_suffix');
        $this->configuration['yaxis_decimal_count'] = $form_state->getValue('yaxis_decimal_count');
        $this->configuration['yaxis_labels_rotation'] = $form_state->getValue('yaxis_labels_rotation');

    }

    /**
     * {@inheritdoc}
     */
    public function build() {

        $options = [
            'library' => $this->configuration['library'],
            'type' => $this->configuration['type'],
            'field_colors'=>$this->configuration['field_colors'],
            'title'=>$this->configuration['title'],
            'title_position'=>$this->configuration['title_position'],
            'data_labels'=>$this->configuration['data_labels'],
            'legend'=>$this->configuration['legend'],
            'legend_position'=>$this->configuration['legend_position'],
            'colors'=>$this->configuration['colors'],
            'background'=>$this->configuration['background'],
            'tooltips'=>$this->configuration['tooltips'],
            'tooltips_use_html'=>$this->configuration['tooltips_use_html'],
            'width'=>$this->configuration['width'],
            'height'=>$this->configuration['height'],
            'xaxis_title'=>$this->configuration['xaxis_title'],
            'xaxis_labels_rotation'=>$this->configuration['xaxis_labels_rotation'],
            'yaxis_title'=>$this->configuration['yaxis_title'],
            'yaxis_min'=>$this->configuration['yaxis_min'],
            'yaxis_max'=>$this->configuration['yaxis_max'],
            'yaxis_prefix'=>$this->configuration['yaxis_prefix'],
            'yaxis_suffix'=>$this->configuration['yaxis_suffix'],
            'yaxis_decimal_count'=>$this->configuration['yaxis_decimal_count'],
            'yaxis_labels_rotation'=>$this->configuration['yaxis_labels_rotation']
        ];

        // Get current hub
        $route_match = \Drupal::service('current_route_match');
        $term = $route_match->getParameter('taxonomy_term');

        /* @var \Drupal\epm\Service\Techhub $techhub */
        $techhub = \Drupal::service('epm.techhub');
        $nps = $techhub->getFeedbackNps([$term->id()]);

        $categories = ['Detractor', 'Passive', 'Promoter'];
        $seriesData = [
            [
                'name'  => 'NPS',
                'color' => '#910000',
                'type'  => NULL,
                'data'  => [$nps['detractor'], $nps['passive'], $nps['promoter']],
            ],
        ];

        return [
            '#theme'      => 'charts_blocks',
            '#library'    => $this->configuration['library'],
            '#categories' => $categories,
            '#seriesData' => $seriesData,
            '#options'    => $options,
            '#id'         => 'techhub_charts_nps_block',
        ];
    }

    /**
     * {@inheritdoc}
     */
    public function getCacheMaxAge() {
        return 0;
    }

}