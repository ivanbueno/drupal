<?php

namespace Drupal\epm\Plugin\Block;

use Drupal\Core\Block\BlockBase;

/**
 * Provides a 'Techhub Charts Productivity' Block.
 *
 * @Block(
 *   id = "techhub_charts_productivity_block",
 *   admin_label = @Translation("Techhub - Charts - Productivity"),
 *   category = @Translation("Techhub"),
 * )
 */
class TechhubChartsProductivityBlock extends BlockBase {


    /**
     * {@inheritdoc}
     */
    public function build() {

        // Get current hub
        $route_match = \Drupal::service('current_route_match');
        $term = $route_match->getParameter('taxonomy_term');

        /* @var \Drupal\epm\Service\Techhub $techhub */
        $techhub = \Drupal::service('epm.techhub');
        if ($term) {
            $nps = $techhub->getFeedbackNps([$term->id()]);
            $productivity = $techhub->getProductivity([$term->id()]);
        }

        return [
            '#markup'     => $term ? '
<div class="bg-dark text-white text-center padding-x-sm">
<div class="container">
    <div class="row">
      <div class="col-xs-12 col-md-5 text-center"><h3><kbd>Productivity: ' . $productivity['time'] . '</kbd></h3></div>
      <div class="col-xs-12 col-md-4 text-center"><h3><kbd>Savings: $' . $productivity['money'] . '</kbd></h3></div>
      <div class="col-xs-12 col-md-3 text-center"><h3><kbd>NPS: ' . round($nps['score']) . '</kbd></h3></div>
    </div>
</div>
</div>
            ' : '',
        ];
    }

    /**
     * {@inheritdoc}
     */
    public function getCacheMaxAge() {
        return 0;
    }

}