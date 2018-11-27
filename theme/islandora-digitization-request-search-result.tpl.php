<?php

/**
* @file
* islandora-digital-workflow-dashboard.tpl display template.
*
* Variables available:
* - $dashboard_data:
*
*/
?>
<div class="lookup_results">
    <?php $toggle = FALSE; ?>
    <?php foreach ($data as $requests_obj): ?>
    <div class="lookup_result <?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
        <span class="small_lt_text">Match found in: <?php print implode(', ', $requests_obj->reasons); ?></span>
        <div class="lookup_result_indent">
          <h2><?php print l($requests_obj->name, '/islandora/islandora_digitization_requests/request/' . $requests_obj->scan_request_id); ?></h2>
          <b>Description:</b> <?php print $requests_obj->description; ?>
        </div>
    </div>
    <?php endforeach; ?>
</div>
<br style="clear:both" />